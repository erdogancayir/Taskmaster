#include <iostream>
#include <map>
#include <string>
#include <csignal>
#include <thread>
#include <unistd.h> // for pause()

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
        // 💥 Become daemon here
        daemonize();

        std::cout << "[taskmasterd] Starting Taskmaster Daemon...\n";
        return EXIT_SUCCESS;

        LogManager::instance().init(logFilePath);

        Config cfg = parseConfig(configFilePath);

        MasterSupervisor master;

        /* --- build supervisors from config --- */
        for (auto& [name, pc] : cfg.programs) {
            auto spec = toSpec(pc);
            master.addProgram(spec);
        }

        /* --- start programs that have autostart=true --- */
        master.startAll();

        /* --- background monitor loop --- */
        std::atomic<bool> running{true};
        std::thread monitor([&] {
            while (running) {
                master.updateAll();
                std::this_thread::sleep_for(std::chrono::milliseconds(500));
            }
        });

         /* --- simple signal handlers for clean exit --- */
        std::signal(SIGINT,  [](int){});
        std::signal(SIGTERM, [](int){});

        /* --- launch socket server (unchanged) --- */
        Server server("/tmp/taskmaster.sock");
        server.start();

        std::cout << "[taskmasterd] Running. Ctrl-C to quit.\n";
        pause();                       // wait for signal

        /* --- shutdown path --- */
        running = false;
        monitor.join();
        master.stopAll();
        server.stop();
    }
    catch(const std::exception& ex)
    {
        std::cerr << "Error: " << ex.what() << std::endl;
        return EXIT_FAILURE;
    }
}