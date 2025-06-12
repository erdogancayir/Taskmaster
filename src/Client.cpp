#include "Client.hpp"
#include <sys/socket.h>
#include <sys/un.h>
#include <unistd.h>
#include <iostream>
#include <cstring>

void startClient(const std::string& socketPath) {
    int fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd == -1) {
        perror("socket"); return;
    }

    sockaddr_un addr{};
    addr.sun_family = AF_UNIX;
    strncpy(addr.sun_path, socketPath.c_str(), sizeof(addr.sun_path) - 1);

    if (connect(fd, (sockaddr*)&addr, sizeof(addr)) == -1) {
        perror("connect"); close(fd); return;
    }

    std::string line;
    std::cout << "Command> ";
    std::getline(std::cin, line);
    write(fd, line.c_str(), line.size());

    char buf[1024]{};
    ssize_t n = read(fd, buf, sizeof(buf)-1);
    if (n > 0) std::cout << "[Client] Reply: " << buf << std::endl;

    close(fd);
}