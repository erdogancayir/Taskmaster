#include "Server.hpp"
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <iostream>
#include <cstring>
#include <csignal>
#include "LogManager.hpp"

Server::Server(const std::string& socketPath) 
    : socketPath(socketPath), serverFd(-1), running(false) {}

Server::~Server()
{
    stop();
}

void Server::start()
{
    serverFd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (serverFd == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    sockaddr_un addr{};
    addr.sun_family = AF_UNIX;
    std::strncpy(addr.sun_path, socketPath.c_str(), sizeof(addr.sun_path) - 1);
    unlink(socketPath.c_str()); // Eski soket varsa sil

    if (bind(serverFd, (struct sockaddr*)&addr, sizeof(addr)) == -1)
    {
        perror("bind");
        close(serverFd);
        exit(EXIT_FAILURE);
    }

    if (listen(serverFd, 5) == -1)
    {
        perror("listen");
        close(serverFd);
        exit(EXIT_FAILURE);
    }

    running = true;
    serverThread = std::thread(&Server::handleConnections, this);
    LogManager::instance().log("Server started on socket: " + socketPath);
}


void Server::stop()
{
    running = false;

    if (serverFd != -1)
        close(serverFd);

    unlink(socketPath.c_str());

    if (serverThread.joinable())
        serverThread.join();
    
    LogManager::instance().log("Server stopped.");
}

void Server::handleConnections()
{
    while (running) {
        int clientFd = accept(serverFd, nullptr, nullptr);
        if (clientFd == -1)
        {
            if (running)
                perror("accept");
            
            continue;
        }
        handleClient(clientFd);
        close(clientFd);
    }
}

void Server::handleClient(int clientFd)
{
    char buffer[1024];
    std::memset(buffer, 0, sizeof(buffer));
    ssize_t n = read(clientFd, buffer, sizeof(buffer) - 1);
    
    if (n > 0)
    {
        std::string command(buffer);
        std::cout << "[Server] Received command: " << command << std::endl;
        LogManager::instance().log("Received command: " + command);

        // Test için sabit cevap
        std::string response = "Command received: " + command;
        write(clientFd, response.c_str(), response.size());
    }
}