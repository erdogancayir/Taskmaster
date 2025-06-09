#pragma once

#include "ProcessManager.hpp"
#include <map>
#include <string>

class Shell {
public:
    explicit Shell(std::map<std::string, ProcessManager>& managers);
    void run();

private:
    std::map<std::string, ProcessManager>& managers;

    void handleCommand(const std::string& input);
    void printStatus() const;
    void reloadConfig();
};