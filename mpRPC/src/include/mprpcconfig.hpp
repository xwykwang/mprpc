#pragma once
#include <unordered_map>
#include <string>

class MpRpcConfig
{
public:
    void LoadConfigFile(const char *config_file);
    std::string Load(const std::string &key);
private:
    std::unordered_map<std::string, std::string> configMap_;
};