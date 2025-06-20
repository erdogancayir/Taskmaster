#include "ConfigParser.hpp"
#include <yaml-cpp/yaml.h>
#include <iostream>

std::map<std::string, ProgramConfig> parseConfig(const std::string& filepath) {
    std::map<std::string, ProgramConfig> configs;

    YAML::Node root = YAML::LoadFile(filepath);
    auto programs = root["programs"];

    for (const auto& it : programs) {
        const std::string& name = it.first.as<std::string>();
        const auto& node = it.second;

        ProgramConfig config;
        config.name = name;
        config.cmd = node["cmd"].as<std::string>();
        config.numProcs = node["numprocs"].as<int>(1);
        config.workingDir = node["workingdir"].as<std::string>("/tmp");
        config.autostart = node["autostart"].as<bool>(false);
        config.autorestart = node["autorestart"].as<std::string>("never");
        config.exitCodes = node["exitcodes"] ? node["exitcodes"].as<std::vector<int>>() : std::vector<int>{0};
        config.startRetries = node["startretries"].as<int>(3);
        config.startTime = node["starttime"].as<int>(5);
        config.stopSignal = node["stopsignal"].as<std::string>("TERM");
        config.stopTime = node["stoptime"].as<int>(10);
        config.stdoutLog = node["stdout"].as<std::string>("");
        config.stderrLog = node["stderr"].as<std::string>("");
        config.umask = node["umask"].as<int>(022);

        if (node["env"]) {
            for (const auto& envVar : node["env"]) {
                config.environment[envVar.first.as<std::string>()] = envVar.second.as<std::string>();
            }
        }

        configs[name] = config;
    }

    return configs;
}