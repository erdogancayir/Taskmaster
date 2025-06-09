#pragma once

#include <string>
#include <fstream>
#include <mutex>

class Logger {
public:
    static void init(const std::string& filename);
    static void log(const std::string& message);

private:
    static std::ofstream logfile;
    static std::mutex logMutex;
};