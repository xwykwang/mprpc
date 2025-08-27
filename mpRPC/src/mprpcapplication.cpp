#include "mprpcapplication.hpp"
#include <unistd.h>

MpRpcConfig MprpcApplication::mpconfig_;

MprpcApplication &MprpcApplication::GetInstance()
{
    static MprpcApplication app;
    return app;
}

MpRpcConfig& MprpcApplication::GetConfig()
{
    return mpconfig_;
}

void ShowArgsHelp()
{
    std::cout<< "format: command -i <configfile>"<<std::endl;
    exit(EXIT_FAILURE);
}
void MprpcApplication::Init(int argc, char **argv)
{
    if(argc < 2)
    {
        ShowArgsHelp();
    }

    int c = 0;
    std::string config_file;
    while((c = getopt(argc, argv, "i:")) != -1)
    {
        switch (c)
        {
        case 'i':
            config_file = optarg;
            break;
        
        default:
            ShowArgsHelp();
            break;
        }
    }

    mpconfig_.LoadConfigFile(config_file.c_str());
    std::cout<< mpconfig_.Load("rpcserverip") <<std::endl;
    std::cout<< mpconfig_.Load("rpcserverport") <<std::endl;
}
