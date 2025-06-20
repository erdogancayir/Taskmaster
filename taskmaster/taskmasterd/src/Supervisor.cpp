#include "Supervisor.hpp"

#include <sstream>
#include <fcntl.h>
#include <unistd.h>
#include <sys/wait.h>
#include <iostream>
#include <cstring>
#include <algorithm>
#include <thread>
#include <chrono>
#include <signal.h>     // for kill(), SIGTERM
#include <sys/stat.h>   // for umask()


ProgramSupervisor::ProgramSupervisor(const ProgramSpec& spec) : spec(spec) {
    processes.resize(spec.numprocs);
}

void ProgramSupervisor::start() {
    for (int i = 0; i < spec.numprocs; ++i) {
        if (processes[i].state == ProcessState::STOPPED || processes[i].state == ProcessState::EXITED) {
            spawnProcess(i);
        }
    }
}

void ProgramSupervisor::stop() {
    for (auto& proc : processes) {
        if (proc.state == ProcessState::RUNNING || proc.state == ProcessState::STARTING) {
            kill(proc.pid, SIGTERM); // TODO: map from spec.stopsignal
            proc.state = ProcessState::STOPPED;
        }
    }
}

void ProgramSupervisor::restart() {
    stop();
    start();
}

void ProgramSupervisor::update() {
    for (int i = 0; i < processes.size(); ++i) {
        monitorProcess(i);
    }
}

const ProgramSpec& ProgramSupervisor::getSpec() const { return spec; }
const std::vector<ProcessInstance>& ProgramSupervisor::getProcesses() const { return processes; }

void ProgramSupervisor::spawnProcess(int index) {
    pid_t pid = fork();
    if (pid == 0) {
        umask(spec.umask);
        if (!spec.workingdir.empty()) chdir(spec.workingdir.c_str());

        if (!spec.stdout_path.empty()) {
            int fd = open(spec.stdout_path.c_str(), O_WRONLY | O_CREAT | O_APPEND, 0644);
            if (fd != -1) dup2(fd, STDOUT_FILENO);
        }
        if (!spec.stderr_path.empty()) {
            int fd = open(spec.stderr_path.c_str(), O_WRONLY | O_CREAT | O_APPEND, 0644);
            if (fd != -1) dup2(fd, STDERR_FILENO);
        }

        std::vector<std::string> args;
        std::istringstream iss(spec.cmd);
        std::string token;
        while (iss >> token) args.push_back(token);

        std::vector<char*> cargs;
        for (auto& arg : args) cargs.push_back(&arg[0]);
        cargs.push_back(nullptr);

        execvp(cargs[0], cargs.data());
        perror("execvp failed");
        exit(EXIT_FAILURE);
    } else if (pid > 0) {
        processes[index].pid = pid;
        processes[index].start_time = std::chrono::steady_clock::now();
        processes[index].state = ProcessState::STARTING;
    } else {
        perror("fork failed");
    }
}

void ProgramSupervisor::monitorProcess(int index) {
    auto& proc = processes[index];
    if (proc.pid <= 0) return;

    int status;
    pid_t result = waitpid(proc.pid, &status, WNOHANG);
    if (result == 0) {
        if (proc.state == ProcessState::STARTING) {
            auto now = std::chrono::steady_clock::now();
            if (std::chrono::duration_cast<std::chrono::seconds>(now - proc.start_time).count() >= spec.starttime) {
                proc.state = ProcessState::RUNNING;
            }
        }
    } else if (result > 0) {
        int code = WIFEXITED(status) ? WEXITSTATUS(status) : -1;
        bool expected = std::find(spec.exitcodes.begin(), spec.exitcodes.end(), code) != spec.exitcodes.end();
        bool shouldRestart =
            (spec.autorestart == RestartPolicy::ALWAYS) ||
            (spec.autorestart == RestartPolicy::UNEXPECTED && !expected);

        if (shouldRestart && proc.retries < spec.startretries) {
            proc.retries++;
            proc.state = ProcessState::BACKOFF;
            spawnProcess(index);
        } else {
            proc.state = expected ? ProcessState::EXITED : ProcessState::FATAL;
        }
    }
}

void MasterSupervisor::addProgram(const ProgramSpec& spec) {
    programs[spec.name] = std::make_shared<ProgramSupervisor>(spec);
}

void MasterSupervisor::removeProgram(const std::string& name) {
    programs.erase(name);
}

void MasterSupervisor::startAll() {
    for (auto& [_, prog] : programs) {
        prog->start();
    }
}

void MasterSupervisor::stopAll() {
    for (auto& [_, prog] : programs) {
        prog->stop();
    }
}

void MasterSupervisor::updateAll() {
    for (auto& [_, prog] : programs) {
        prog->update();
    }
}

std::map<std::string, std::shared_ptr<ProgramSupervisor>> MasterSupervisor::getPrograms() const {
    return programs;
}