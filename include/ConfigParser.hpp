#pragma once

#include <string>
#include <map>
#include <vector>

struct ProgramConfig {
    std::string name;
    std::string cmd;
    int numProcs = 1;
    std::string workingDir;
    bool autostart = false;
    std::string autorestart; // "always", "never", "unexpected"
    std::vector<int> exitCodes;
    int startRetries = 3;
    int startTime = 5;
    std::string stopSignal = "TERM";
    int stopTime = 10;
    std::string stdoutLog;
    std::string stderrLog;
    std::map<std::string, std::string> environment;
    int umask = 022;
};

std::map<std::string, ProgramConfig> parseConfig(const std::string& filepath);