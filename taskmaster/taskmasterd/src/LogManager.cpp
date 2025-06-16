#include "LogManager.hpp"
#include <iostream>
#include <chrono>
#include <iomanip>
#include <filesystem>

LogManager::~LogManager()
{
    if (logFile.is_open())
        logFile.close();
}

LogManager &LogManager::instance()
{
    static LogManager instance;

    return instance;
}

void LogManager::init(const std::string& filePath)
{
    this->filePath = filePath;

    std::filesystem::create_directories(std::filesystem::path(filePath).parent_path());

    logFile.open(filePath, std::ios::app);
    
    if (!logFile.is_open())
        throw std::runtime_error("Could not open log file: " + this->filePath);
}

void LogManager::log(const std::string& message)
{
    std::lock_guard<std::mutex> lock(mtx);

    auto now = std::chrono::system_clock::now();
    auto t = std::chrono::system_clock::to_time_t(now);
    auto localTime = *std::localtime(&t);

    logFile << "[" << std::put_time(&localTime, "%Y-%m-%d %H:%M:%S") << "] "
            << message << std::endl;
}