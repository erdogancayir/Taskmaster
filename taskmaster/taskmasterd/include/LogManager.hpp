#pragma once
#include <fstream>
#include <string>
#include <mutex>

class LogManager {
public:
    static LogManager& instance(); // singleton

    void log(const std::string& message);
    void init(const std::string& filepath); // çağrılmalı

private:
    LogManager() = default;
    ~LogManager();

    std::ofstream logFile;
    std::mutex mtx;
    std::string filePath;
};