#pragma once

#include <string>

class Client {
public:
    Client(const std::string& socketPath);
    bool sendCommand(const std::string& command, std::string& response);

private:
    std::string socketPath;
};