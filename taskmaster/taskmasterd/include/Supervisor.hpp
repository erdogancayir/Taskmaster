#pragma once

#include <string>
#include <vector>
#include <map>
#include <memory>
#include <chrono>
#include <optional>

enum class RestartPolicy { NEVER, ALWAYS, UNEXPECTED };

enum class ProcessState {
    STOPPED, STARTING, RUNNING, BACKOFF, EXITED, FATAL
};

struct ProgramSpec {
    std::string name;
    std::string cmd;
    int numprocs = 1;
    int umask = 022;
    std::string workingdir;
    bool autostart = false;
    RestartPolicy autorestart = RestartPolicy::UNEXPECTED;
    std::vector<int> exitcodes = {0};
    int startretries = 3;
    int starttime = 1;
    std::string stopsignal = "TERM";
    int stoptime = 5;
    std::string stdout_path;
    std::string stderr_path;
    std::map<std::string, std::string> environment;
};

struct ProcessInstance {
    pid_t pid = -1;
    ProcessState state = ProcessState::STOPPED;
    std::chrono::steady_clock::time_point start_time;
    int retries = 0;
};

class ProgramSupervisor {
public:
    ProgramSupervisor(const ProgramSpec& spec);
    void start();
    void stop();
    void restart();
    void update();
    const ProgramSpec& getSpec() const;
    const std::vector<ProcessInstance>& getProcesses() const;

private:
    void spawnProcess(int index);
    void monitorProcess(int index);

    ProgramSpec spec;
    std::vector<ProcessInstance> processes;
};

class MasterSupervisor {
public:
    void addProgram(const ProgramSpec& spec);
    void removeProgram(const std::string& name);
    void startAll();
    void stopAll();
    void updateAll();
    std::map<std::string, std::shared_ptr<ProgramSupervisor>> getPrograms() const;

private:
    std::map<std::string, std::shared_ptr<ProgramSupervisor>> programs;
};

void daemonize();