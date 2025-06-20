#include "Config.hpp"
#include <yaml-cpp/yaml.h>
#include <stdexcept>
#include <iostream>

ProgramSpec toSpec(const ProgramConfig& pc)
{
    ProgramSpec s;
    s.name         = pc.name;
    s.cmd          = pc.cmd;
    s.numprocs     = pc.numprocs;
    s.umask        = pc.umask;
    s.workingdir   = pc.workingdir;
    s.autostart    = pc.autostart;

    // map “autorestart” string → enum
    if (pc.autorestart == "always")      s.autorestart = RestartPolicy::ALWAYS;
    else if (pc.autorestart == "never")  s.autorestart = RestartPolicy::NEVER;
    else                                 s.autorestart = RestartPolicy::UNEXPECTED;

    s.exitcodes    = pc.exitcodes;
    s.startretries = pc.startretries;
    s.starttime    = pc.starttime;
    s.stopsignal   = pc.stopsignal;
    s.stoptime     = pc.stoptime;
    s.stdout_path  = pc.stdout_log;
    s.stderr_path  = pc.stderr_log;
    s.environment  = pc.env;
    return s;
}

Config parseConfig(const std::string& filePath)
{
    YAML::Node root = YAML::LoadFile(filePath);
    Config config;

    if (!root["programs"])
        throw std::runtime_error("Missing 'programs' section in config file");

    for (const auto& programNode : root["programs"])
    {
        const std::string& name = programNode.first.as<std::string>();
        const auto& node = programNode.second;

        ProgramConfig pc;
        pc.name = name;
        pc.cmd = node["cmd"].as<std::string>();
        pc.numprocs = node["numprocs"].as<int>(1);
        pc.workingdir = node["workingdir"].as<std::string>("/tmp");
        pc.autostart = node["autostart"].as<bool>(false);
        pc.autorestart = node["autorestart"].as<std::string>("never");
        pc.exitcodes = node["exitcodes"] ? node["exitcodes"].as<std::vector<int>>() : std::vector<int>{0};
        pc.startretries = node["startretries"].as<int>(3);
        pc.starttime = node["starttime"].as<int>(5);
        pc.stopsignal = node["stopsignal"].as<std::string>("TERM");
        pc.stoptime = node["stoptime"].as<int>(10);
        pc.stdout_log = node["stdout"].as<std::string>("");
        pc.stderr_log = node["stderr"].as<std::string>("");
        pc.umask = node["umask"].as<int>(022);

        if (node["env"])
        {
            for (const auto& envVar : node["env"])
            {
                pc.env[envVar.first.as<std::string>()] = envVar.second.as<std::string>();
            }
        }

        config.programs[name] = pc;
    }

    return  std::move(config); // Return Value Optimization - Move Semantics
}

bool parseArguments(int ac, char **av, std::string &configFile, std::string &logFile)
{
    if (ac != 5)
    {
        std::cerr << "Usage: " << " --cf <config_file> --lf <log_file>" << std::endl;
        return false;
    }

    for (int i = 1; i < ac; i += 2)
    {
        std::string flag = av[i];
        std::string value = av[i + 1];

        if (flag == "--cf")
            configFile = value;
        else if (flag == "--lf")
            logFile = value;
        else
        {
            std::cerr << "Unknown flag: " << flag << std::endl;
            return false;
        }
    }

    if (configFile.empty() || logFile.empty())
    {
        std::cerr << "Error: Both --cf and --lf must be provided." << std::endl;
        return false;
    }

    return true;
}