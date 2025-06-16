#include "Client.hpp"
#include <iostream>
#include <string>

int main() {
    Client client("/tmp/taskmaster.sock");

    std::cout << "[taskmasterctl] Connected to daemon.\n";
    while (true) {
        std::cout << ">> ";
        std::string input;
        if (!std::getline(std::cin, input))
            break;

        if (input == "exit")
            break;

        std::string response;
        if (client.sendCommand(input, response)) {
            std::cout << "[Response] " << response << "\n";
        } else {
            std::cout << "[Error] Could not send command.\n";
        }
    }

    std::cout << "Exiting taskmasterctl.\n";
    return 0;
}