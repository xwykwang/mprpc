#include "mprpcconfig.hpp"
#include <iostream>

void MpRpcConfig::LoadConfigFile(const char* config_file)
{
    FILE* pf = fopen(config_file, "r");
    if(pf == nullptr)
    {
        std::cout<< "config file not exist!"<< std::endl;
        exit(EXIT_FAILURE);
    }

    while(!feof(pf))
    {
        char buf[512] = {0};
        fgets(buf, 512, pf);
        std::string cnf = buf;
        int idx = cnf.find('=');
        int end = cnf.find('\n');
        if(idx == -1)
        {
            continue;
        }
        std::string key = cnf.substr(0, idx);
        std::string value = cnf.substr(idx+1, end-idx-1);
        configMap_.insert({key, value});
    }
}

std::string MpRpcConfig::Load(const std::string &key)
{
    if(configMap_.find(key) != configMap_.end())
    {
        return configMap_[key];
    }
    return "";
}