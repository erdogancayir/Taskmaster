#include "CommandHandler.hpp"
#include <sstream>

CommandHandler::CommandHandler(std::map<std::string, ProcessManager>& mgrs)
    : managers(mgrs) {}

std::string CommandHandler::handleCommand(const std::string& command) {
    std::istringstream iss(command);
    std::string cmd, name;
    iss >> cmd;
    iss >> name;

    if (cmd == "start") return handleStart(name);
    if (cmd == "stop") return handleStop(name);
    if (cmd == "restart") return handleRestart(name);
    if (cmd == "status") {
        if (name.empty()) return handleStatusAll();
        return handleStatus(name);
    }
    return "Unknown command.";
}

std::string CommandHandler::handleStart(const std::string& name) {
    auto it = managers.find(name);
    if (it == managers.end()) return "Program not found.";
    it->second.start();
    return "Started: " + name;
}

std::string CommandHandler::handleStop(const std::string& name) {
    auto it = managers.find(name);
    if (it == managers.end()) return "Program not found.";
    it->second.stop();
    return "Stopped: " + name;
}

std::string CommandHandler::handleRestart(const std::string& name) {
    auto it = managers.find(name);
    if (it == managers.end()) return "Program not found.";
    it->second.restart();
    return "Restarted: " + name;
}

std::string CommandHandler::handleStatus(const std::string& name) {
    auto it = managers.find(name);
    if (it == managers.end()) return "Program not found.";
    return name + ": " + (it->second.isRunning() ? "Running" : "Stopped");
}

std::string CommandHandler::handleStatusAll() {
    std::string result;
    for (const auto& [name, mgr] : managers) {
        result += name + ": " + (mgr.isRunning() ? "Running" : "Stopped") + "\n";
    }
    return result;
}