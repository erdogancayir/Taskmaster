#include <iostream>
#include <map>
#include <string>
#include "ConfigParser.hpp"

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
    try {
        const std::string configFile = "../src/config/taskmaster.yaml";
        auto configs = parseConfig(configFile);

        for (const auto& [name, config] : configs) {
            printConfig(config);
        }

        // Buradan sonra ProcessManager ve Shell entegre edilecek...

    } catch (const std::exception& ex) {
        std::cerr << "Error: " << ex.what() << std::endl;
        return 1;
    }

    return 0;
}