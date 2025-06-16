#include <iostream>
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>

#define SOCKET_PATH "/tmp/taskmaster.sock"

void startClient() {
    int sock = socket(AF_UNIX, SOCK_STREAM, 0);
    if (sock < 0) {
        perror("socket");
        return;
    }

    sockaddr_un addr;
    addr.sun_family = AF_UNIX;
    strcpy(addr.sun_path, SOCKET_PATH);

    if (connect(sock, (sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("connect");
        close(sock);
        return;
    }

    std::string input;
    std::cout << "Enter command: ";
    while (std::getline(std::cin, input)) {
        if (send(sock, input.c_str(), input.size(), 0) < 0) {
            perror("send");
            break;
        }

        char buffer[1024] = {0};
        ssize_t n = recv(sock, buffer, sizeof(buffer), 0);
        if (n > 0) {
            std::cout << "Response: " << buffer << std::endl;
        }

        std::cout << "Enter command: ";
    }

    close(sock);
}