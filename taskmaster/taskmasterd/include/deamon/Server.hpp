// daemon/Server.hpp
#pragma once

#include <string>
#include <thread>
#include <atomic>

class Server {
public:
    Server(const std::string& socketPath);
    ~Server();

    void start();
    void stop();

private:
    std::string socketPath;
    int serverFd;
    std::thread serverThread;
    std::atomic<bool> running;

    void handleConnections();
    void handleClient(int clientFd);
};