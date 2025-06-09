#include "ProcessManager.hpp"
#include <unistd.h>
#include <sys/wait.h>
#include <iostream>
#include <fcntl.h>
#include <cstring>
#include <csignal>
#include <sstream>      // std::istringstream
#include <sys/stat.h>   // umask

#include "Logger.hpp"
#include <sys/wait.h>
#include <chrono>
#include <thread>

ProcessManager::ProcessManager(const ProgramConfig& cfg) : config(cfg) {}

ProcessManager::~ProcessManager() {
    stop();
}

void ProcessManager::start() {
    for (int i = 0; i < config.numProcs; ++i) {
        launchProcess();
    }
}

void ProcessManager::launchProcess() {
    pid_t pid = fork();
    if (pid < 0) {
        std::cerr << "Fork failed.\n";
        return;
    }
    if (pid == 0) {
        // child

        // Working directory
        if (!config.workingDir.empty()) {
            chdir(config.workingDir.c_str());
        }

        // Umask
        umask(config.umask);

        // Env
        for (const auto& [key, val] : config.environment) {
            setenv(key.c_str(), val.c_str(), 1);
        }

        // stdout / stderr
        if (!config.stdoutLog.empty()) {
            int out = open(config.stdoutLog.c_str(), O_CREAT | O_WRONLY | O_APPEND, 0644);
            dup2(out, STDOUT_FILENO);
        }
        if (!config.stderrLog.empty()) {
            int err = open(config.stderrLog.c_str(), O_CREAT | O_WRONLY | O_APPEND, 0644);
            dup2(err, STDERR_FILENO);
        }

        std::vector<char*> args;
        std::istringstream iss(config.cmd);
        std::string word;
        while (iss >> word) {
            args.push_back(strdup(word.c_str()));
        }
        args.push_back(nullptr);

        execvp(args[0], args.data());

        // execvp failed:
        std::cerr << "Exec failed: " << strerror(errno) << "\n";
        exit(1);
    } else {
        // Ana süreç
        processes.push_back({
            pid,
            true,
            0,
            std::chrono::steady_clock::now(),
            false
        });
        std::cout << "Started process " << config.name << " (PID " << pid << ")\n";
    }
}

int signalNameToInt(const std::string& name) {
    if (name == "TERM") return SIGTERM;
    if (name == "INT")  return SIGINT;
    if (name == "KILL") return SIGKILL;
    if (name == "USR1") return SIGUSR1;
    if (name == "USR2") return SIGUSR2;
    // other signals
    return SIGTERM; // default
}

void ProcessManager::stop() {
    int sig = signalNameToInt(config.stopSignal);

    for (auto& proc : processes)
    {
        Logger::log("Stopping process " + std::to_string(proc.pid) + " with signal " + config.stopSignal);
        kill(proc.pid, sig);

        // wait until stoptime
        int waited = 0;
        while (waited < config.stopTime) {
            int status;
            pid_t result = waitpid(proc.pid, &status, WNOHANG);
            if (result > 0) {
                proc.running = false;
                Logger::log("Process " + std::to_string(proc.pid) + " exited gracefully.");
                break;
            }
            std::this_thread::sleep_for(std::chrono::seconds(1));
            ++waited;
        }

        if (proc.running)
        {
            kill(proc.pid, SIGTERM);
            waitpid(proc.pid, nullptr, 0);
            proc.running = false;
            std::cout << "Stopped process PID " << proc.pid << "\n";
        }
    }
}

void ProcessManager::restart() {
    stop();
    start();
}

bool ProcessManager::isRunning() const {
    for (const auto& proc : processes) {
        if (proc.running) return true;
    }
    return false;
}

const ProgramConfig& ProcessManager::getConfig() const {
    return config;
}

void ProcessManager::checkProcesses() {
    for (auto& proc : processes) {
        if (!proc.running) continue;

        int status;
        pid_t result = waitpid(proc.pid, &status, WNOHANG);

        if (result == 0) {
            // still working
            continue;
        }

        proc.running = false;
        int exitCode = WIFEXITED(status) ? WEXITSTATUS(status) : -1;

        Logger::log(config.name + " (PID " + std::to_string(proc.pid) + ") exited with code " + std::to_string(exitCode));

        bool expected = std::find(config.exitCodes.begin(), config.exitCodes.end(), exitCode) != config.exitCodes.end();
        bool shouldRestart = false;

        if (config.autorestart == "always") {
            shouldRestart = true;
        } else if (config.autorestart == "unexpected" && !expected) {
            shouldRestart = true;
        }

        if (shouldRestart) {
            if (proc.restartAttempts < config.startRetries) {
                Logger::log("Restarting " + config.name + "...");
                ++proc.restartAttempts;

                std::this_thread::sleep_for(std::chrono::seconds(1));
                launchProcess();
            } else {
                Logger::log("Maximum retries reached for " + config.name);
            }
        }
    }
}