#include "Server.hpp"
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <iostream>
#include <cstring>

#include <iostream>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <cstring>
#include <thread>

#define SOCKET_PATH "/tmp/taskmaster.sock"

void handleClient(int clientSocket) {
    char buffer[1024];
    while (true) {
        ssize_t n = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);
        if (n <= 0) break;
        buffer[n] = '\0';
        std::cout << "Received: " << buffer << std::endl;

        std::string response = "ACK: ";
        response += buffer;
        send(clientSocket, response.c_str(), response.size(), 0);
    }
    close(clientSocket);
}

void startServer() {
    unlink(SOCKET_PATH);

    int serverSocket = socket(AF_UNIX, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        perror("socket");
        return;
    }

    sockaddr_un addr;
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, SOCKET_PATH);

    if (bind(serverSocket, (sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        close(serverSocket);
        return;
    }

    if (listen(serverSocket, 5) < 0) {
        perror("listen");
        close(serverSocket);
        return;
    }

    std::cout << "Server listening on " << SOCKET_PATH << std::endl;

    while (true) {
        int clientSocket = accept(serverSocket, nullptr, nullptr);
        if (clientSocket < 0) {
            perror("accept");
            continue;
        }
        std::thread(handleClient, clientSocket).detach();
    }

    close(serverSocket);
}