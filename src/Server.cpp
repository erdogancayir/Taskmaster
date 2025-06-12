#include "Server.hpp"
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <iostream>
#include <cstring>

void startServer(const std::string& socketPath) {
    int serverFd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (serverFd == -1) {
        perror("socket"); return;
    }

    sockaddr_un addr{};
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, socketPath.c_str(), sizeof(addr.sun_path) - 1);
    unlink(socketPath.c_str());

    if (bind(serverFd, (sockaddr*)&addr, sizeof(addr)) == -1) {
        perror("bind"); close(serverFd); return;
    }
    if (listen(serverFd, 5) == -1) {
        perror("listen"); close(serverFd); return;
    }

    std::cout << "[Server] Listening on " << socketPath << std::endl;

    while (true) {
        int clientFd = accept(serverFd, nullptr, nullptr);
        if (clientFd == -1) continue;

        char buf[1024]{};
        ssize_t n = read(clientFd, buf, sizeof(buf)-1);
        if (n > 0) {
            std::cout << "[Server] Received: " << buf << std::endl;
            std::string reply = "Command received: ";
            reply += buf;
            write(clientFd, reply.c_str(), reply.size());
        }
        close(clientFd);
    }
}