#pragma once

#include "ProcessManager.hpp"
#include <map>
#include <string>

class CommandHandler {
public:
    explicit CommandHandler(std::map<std::string, ProcessManager>& managers);

    std::string handleCommand(const std::string& command);

private:
    std::map<std::string, ProcessManager>& managers;

    std::string handleStart(const std::string& name);
    std::string handleStop(const std::string& name);
    std::string handleRestart(const std::string& name);
    std::string handleStatus(const std::string& name);
    std::string handleStatusAll();
};