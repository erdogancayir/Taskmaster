#pragma once

#include "ConfigParser.hpp"
#include <vector>
#include <sys/types.h>

struct ProcessInstance {
    pid_t pid;
    bool running;
    int restartAttempts = 0;
    std::chrono::steady_clock::time_point startTime;  // ✅ start time
    bool confirmed = false;  // ✅ is it passed the starttime
};

class ProcessManager {
public:
    explicit ProcessManager(const ProgramConfig& config);
    ~ProcessManager();

    void start();
    void stop();
    void restart();
    bool isRunning() const;

    const ProgramConfig& getConfig() const;
    void checkProcesses();

private:
    ProgramConfig config;
    std::vector<ProcessInstance> processes;

    void launchProcess();
};