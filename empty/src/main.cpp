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

int main() {
    signal(SIGHUP, handleSIGHUP);

    try {
        Logger::init("taskmaster.log");

        const std::string configFile = "../src/config/taskmaster.yaml";
        auto configs = parseConfig(configFile);

        std::map<std::string, ProcessManager> managers;

        for (const auto& [name, config] : configs) {
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