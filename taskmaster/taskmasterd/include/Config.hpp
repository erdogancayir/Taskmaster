#pragma once

#include <string>
#include <map>

#include "Supervisor.hpp"

struct ProgramConfig
{
    std::string name;
    std::string cmd;
    int numprocs = 1;
    int umask = 022;
    std::string workingdir;
    bool autostart = true;
    std::string autorestart = "unexpected"; // "always", "never"
    std::vector<int> exitcodes;
    int startretries = 3;
    int starttime = 1;
    std::string stopsignal = "TERM";
    int stoptime = 10;
    std::string stdout_log;
    std::string stderr_log;
    std::map<std::string, std::string> env;
};

struct Config
{
    std::map<std::string, ProgramConfig> programs;
};

Config parseConfig(const std::string& filePath);
bool parseArguments(int ac, char **av, std::string &configFile, std::string &logFile);
ProgramSpec toSpec(const ProgramConfig& pc);