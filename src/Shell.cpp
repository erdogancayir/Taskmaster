#include "Shell.hpp"
#include <iostream>
#include <sstream>
#include "Logger.hpp"
#include "ConfigParser.hpp"
#include "ProcessManager.hpp"
#include <csignal>
#include <atomic>
#include <thread>

extern std::atomic<bool> reloadRequested;

Shell::Shell(std::map<std::string, ProcessManager>& mgrs) : managers(mgrs) {}

void Shell::run() {
    std::string input;
    std::cout << "taskmaster> ";
    while (std::getline(std::cin, input)) {
        handleCommand(input);

        for (auto& [name, manager] : managers) {
            manager.checkProcesses();
        }

        if (reloadRequested) {
            Logger::log("SIGHUP received: Reloading configuration...");
            reloadConfig();
            reloadRequested = false;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
        std::cout << "taskmaster> " << std::flush;
    }
}

void Shell::handleCommand(const std::string& input) {
    std::istringstream iss(input);
    std::string command, arg;
    iss >> command >> arg;

    if (command == "status") {
        printStatus();
    } else if (command == "start") {
        auto it = managers.find(arg);
        if (it != managers.end()) {
            it->second.start();
            std::cout << "Started: " << arg << "\n";
        } else {
            std::cout << "No such program: " << arg << "\n";
        }
    } else if (command == "stop") {
        auto it = managers.find(arg);
        if (it != managers.end()) {
            it->second.stop();
            std::cout << "Stopped: " << arg << "\n";
        } else {
            std::cout << "No such program: " << arg << "\n";
        }
    } else if (command == "restart") {
        auto it = managers.find(arg);
        if (it != managers.end()) {
            it->second.restart();
            std::cout << "Restarted: " << arg << "\n";
        } else {
            std::cout << "No such program: " << arg << "\n";
        }
    }
    else if (command == "quit") {
        std::exit(0);
    } else {
        std::cout << "Unknown command.\n";
    }
}

void Shell::printStatus() const {
    for (const auto& [name, manager] : managers) {
        std::cout << name << ": " << (manager.isRunning() ? "Running" : "Stopped") << "\n";
    }
}


void Shell::reloadConfig() {
    const std::string configFile = "../src/config/taskmaster.yaml";
    auto newConfigs = parseConfig(configFile);

    for (const auto& [name, newConfig] : newConfigs) {
        auto it = managers.find(name);
        if (it == managers.end()) {
            Logger::log("New program added: " + name);
            managers.emplace(name, newConfig);
            if (newConfig.autostart)
            {
                auto it = managers.find(name);
                if (it != managers.end()) {
                    it->second.start();
                }
            }
        } else {
            // there is same name but setting can be changed
            if (it->second.getConfig().cmd != newConfig.cmd) {
                Logger::log("Program changed: " + name);
                it->second.stop();
                it->second = ProcessManager(newConfig);
                it->second.start();
            }
        }
    }

    // stop deleted programs
    for (auto it = managers.begin(); it != managers.end(); ) {
        if (newConfigs.find(it->first) == newConfigs.end()) {
            Logger::log("Program removed: " + it->first);
            it->second.stop();
            it = managers.erase(it);
        } else {
            ++it;
        }
    }

    Logger::log("Configuration reload complete.");
}