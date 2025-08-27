#pragma once
#include "google/protobuf/service.h"
#include "mprpcconfig.hpp"

class MprpcApplication
{
public:
    static void Init(int argc, char** argv);
    static MprpcApplication& GetInstance();
    static MpRpcConfig& GetConfig();

private:
    static MpRpcConfig mpconfig_;
    MprpcApplication(){};
    MprpcApplication(const MprpcApplication&) = delete;
    MprpcApplication(MprpcApplication &&) = delete;
};