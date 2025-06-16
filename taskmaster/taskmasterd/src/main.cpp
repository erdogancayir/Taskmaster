#include <iostream>
#include <map>
#include <string>

#include "Config.hpp"
#include "LogManager.hpp"
#include "Server.hpp"

int main (int ac, char **av)
{
    std::string configFilePath;
    std::string logFilePath;

    if (!parseArguments(ac, av, configFilePath, logFilePath))
        return EXIT_FAILURE;

    try
    {
        LogManager::instance().init(logFilePath);
        LogManager::instance().log("Taskmaster starting with config: " + configFilePath);

        Config config = parseConfig(configFilePath);
        LogManager::instance().log("Configuration parsed successfully.");

        Server server("/tmp/taskmaster.sock");
        server.start();

            std::cout << "[Taskmasterd] Running. Press Enter to quit.\n";
            std::cin.get(); // Bekle

            server.stop();
    }
    catch(const std::exception& ex)
    {
        std::cerr << "Error: " << ex.what() << std::endl;
        return EXIT_FAILURE;
    }
}