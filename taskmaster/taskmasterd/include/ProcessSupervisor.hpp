#pragma once
#include "Config.hpp"
#include "ManagedProcess.hpp"
#include <vector>
#include <memory>
#include <string>

class ProcessSupervisor {
public:
    explicit ProcessSupervisor(const Config& config);
    void start();
    void stop();
    void restart();
    void status() const;

private:
    Config config;
    std::vector<std::unique_ptr<ManagedProcess>> processes;
};