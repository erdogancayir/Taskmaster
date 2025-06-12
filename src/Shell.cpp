#include "Shell.hpp"
#include <iostream>
#include <sstream>
#include "Logger.hpp"
#include "ConfigParser.hpp"
#include <csignal>
#include <atomic>
#include <thread>

extern std::atomic<bool> reloadRequested;

Shell::Shell(std::map<std::string, ProcessManager>& mgrs)
    : managers(mgrs), commandHandler(mgrs) {}

void Shell::run() {
    std::string input;
    while (true) {
        std::cout << "taskmaster> ";

        if (std::cin.peek() != EOF) {
            std::getline(std::cin, input);
            handleCommand(input);
        }

        for (auto& [name, manager] : managers) {
            manager.checkProcesses();
        }

        if (reloadRequested) {
            Logger::log("SIGHUP received: Reloading configuration...");
            reloadConfig();
            reloadRequested = false;
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(500));
    }
}

void Shell::handleCommand(const std::string& input) {
    if (input == "quit") {
        std::exit(0);
    }

    std::string output = commandHandler.handleCommand(input);
    std::cout << output << std::endl;
}

void Shell::reloadConfig() {
    const std::string configFile = "../src/config/taskmaster.yaml";
    auto newConfigs = parseConfig(configFile);

    for (const auto& [name, newConfig] : newConfigs) {
        auto it = managers.find(name);
        if (it == managers.end()) {
            Logger::log("New program added: " + name);
            managers.emplace(name, newConfig);
            if (newConfig.autostart) {
                auto it = managers.find(name);
                if (it != managers.end()) {
                    it->second.start();
                }
            }
        } else {
            if (it->second.getConfig().cmd != newConfig.cmd) {
                Logger::log("Program changed: " + name);
                it->second.stop();
                it->second = ProcessManager(newConfig);
                it->second.start();
            }
        }
    }

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