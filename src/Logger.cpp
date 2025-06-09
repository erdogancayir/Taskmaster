#include "Logger.hpp"
#include <ctime>
#include <iostream>

std::ofstream Logger::logfile;
std::mutex Logger::logMutex;

void Logger::init(const std::string& filename) {
    logfile.open(filename, std::ios::app);
    if (!logfile) {
        std::cerr << "Failed to open log file: " << filename << std::endl;
    }
}

void Logger::log(const std::string& message) {
    std::lock_guard<std::mutex> lock(logMutex);
    if (!logfile.is_open()) return;

    std::time_t now = std::time(nullptr);
    char buf[32];
    std::strftime(buf, sizeof(buf), "%F %T", std::localtime(&now));

    logfile << "[" << buf << "] " << message << std::endl;
}