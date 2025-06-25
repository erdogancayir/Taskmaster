#include <iostream>
#include <map>
#include <string>
#include <csignal>
#include <thread>
#include <unistd.h> // for pause()

#include "Config.hpp"
#include "LogManager.hpp"
#include "Server.hpp"

int main(int ac, char** av)
{
    std::string cfgPath, logPath;
    if (!parseArguments(ac, av, cfgPath, logPath))
        return EXIT_FAILURE;

    daemonize();

    try {
        LogManager::instance().init(logPath);
        LogManager::instance().log("Taskmaster daemon starting with " + cfgPath);

        Config cfg = parseConfig(cfgPath);
        MasterSupervisor master;
        for (auto& [_, pc] : cfg.programs) master.addProgram(toSpec(pc));
        master.startAll();

        std::atomic<bool> running{true};
        std::thread monitor([&]{
            while (running) {
                master.updateAll();
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
            }
        });

        auto stopHandler = [&](int){ running = false; };
        std::signal(SIGINT,  +[](int sig){  });
        std::signal(SIGTERM, +[](int sig){  });

        Server server("/tmp/taskmaster.sock");
        server.start();

        while (running) pause();     // sleep until a signal flips the flag

        server.stop();
        master.stopAll();
        monitor.join();
    }
    catch (const std::exception& ex) {
        // LogManager may not be ready, so write to stderr as fallback
        std::cerr << "Error: " << ex.what() << '\n';
        return EXIT_FAILURE;
    }
}