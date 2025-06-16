#pragma once

#include "ProcessManager.hpp"
#include <map>
#include <string>
#include "CommandHandler.hpp"

class Shell {
public:
    explicit Shell(std::map<std::string, ProcessManager>& managers);
    void run();

private:
    std::map<std::string, ProcessManager>& managers;
    CommandHandler commandHandler;

    void handleCommand(const std::string& input);
    void printStatus() const;
    void reloadConfig();
};