#include <iostream>
#include <map>
#include <string>
#include "ConfigParser.hpp"
#include "ProcessManager.hpp"
#include "Shell.hpp"
#include "Logger.hpp"

#include <csignal>
#include <atomic>

std::atomic<bool> reloadRequested = false;

void handleSIGHUP(int) {
    reloadRequested = true;
}

void printConfig(const ProgramConfig& config) {
    std::cout << "Program: " << config.name << "\n";
    std::cout << "  Command: " << config.cmd << "\n";
    std::cout << "  NumProcs: " << config.numProcs << "\n";
    std::cout << "  WorkingDir: " << config.workingDir << "\n";
    std::cout << "  Autostart: " << (config.autostart ? "yes" : "no") << "\n";
    std::cout << "  Autorestart: " << config.autorestart << "\n";
    std::cout << "  ExitCodes: ";
    for (int code : config.exitCodes) std::cout << code << " ";
    std::cout << "\n";
    std::cout << "  StartRetries: " << config.startRetries << "\n";
    std::cout << "  StartTime: " << config.startTime << "\n";
    std::cout << "  StopSignal: " << config.stopSignal << "\n";
    std::cout << "  StopTime: " << config.stopTime << "\n";
    std::cout << "  Umask: " << std::oct << config.umask << std::dec << "\n";
    std::cout << "  Stdout: " << config.stdoutLog << "\n";
    std::cout << "  Stderr: " << config.stderrLog << "\n";
    std::cout << "  Env:\n";
    for (const auto& [key, val] : config.environment) {
        std::cout << "    " << key << "=" << val << "\n";
    }
    std::cout << std::endl;
}

int main() {
    signal(SIGHUP, handleSIGHUP);

    try {
        Logger::init("taskmaster.log");

        const std::string configFile = "../src/config/taskmaster.yaml";
        auto configs = parseConfig(configFile);

        std::map<std::string, ProcessManager> managers;

        for (const auto& [name, config] : configs) {
            printConfig(config);
            managers.emplace(name, config);

            if (config.autostart) {
                auto it = managers.find(name);
                if (it != managers.end()) {
                    it->second.start();
                }
            }
        }

        Shell shell(managers);
        shell.run();

    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        return 1;
    }

    return 0;
}