#pragma once

#include <string>
#include <thread>
#include <atomic>
#include <chrono>
#include <vector>
#include <map>

class ManagedProcess {
public:
    ManagedProcess(const std::string& name, const std::string& cmd, const std::string& workingDir,
                   const std::map<std::string, std::string>& envVars, const std::string& stdoutPath,
                   const std::string& stderrPath, int umaskValue, int starttime, int stoptime,
                   const std::string& stopsignal, const std::vector<int>& expectedExitCodes);

    bool start();
    void monitor(); // thread
    void stop();
    bool isRunning() const;

    pid_t getPid() const;

private:
    std::string name;
    std::string cmd;
    std::string workingDir;
    std::map<std::string, std::string> envVars;
    std::string stdoutPath;
    std::string stderrPath;
    int umaskValue;
    int starttime;
    int stoptime;
    std::string stopsignal;
    std::vector<int> expectedExitCodes;

    pid_t pid;
    std::thread monitorThread;
    std::atomic<bool> running;
};