#include "Client.hpp"
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <iostream>
#include <cstring>

Client::Client(const std::string& path) : socketPath(path) {}

bool Client::sendCommand(const std::string& command, std::string& response) {
    int clientFd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (clientFd == -1) {
        perror("socket");
        return false;
    }

    sockaddr_un addr{};
    addr.sun_family = AF_UNIX;
    std::strncpy(addr.sun_path, socketPath.c_str(), sizeof(addr.sun_path) - 1);

    if (connect(clientFd, (struct sockaddr*)&addr, sizeof(addr)) == -1) {
        perror("connect");
        close(clientFd);
        return false;
    }

    if (write(clientFd, command.c_str(), command.size()) == -1) {
        perror("write");
        close(clientFd);
        return false;
    }

    char buffer[1024];
    std::memset(buffer, 0, sizeof(buffer));
    ssize_t n = read(clientFd, buffer, sizeof(buffer) - 1);
    if (n > 0) {
        response = buffer;
    } else {
        response = "[Client] No response or read error.";
    }

    close(clientFd);
    return true;
}