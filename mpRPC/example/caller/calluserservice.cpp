#include <iostream>
#include "user.pb.h"
#include "mprpcapplication.hpp"
#include "mprpcchannel.hpp"

int main(int argc, char **argv)
{
    MprpcApplication::Init(argc, argv);

    fixbug::UserServiceRpc_Stub stub(new MprpcChannel());

    fixbug::LoginRequest request;
    fixbug::LoginResponse response;
    request.set_name("zhangsan");
    request.set_pwd("123456");    

    stub.Login(nullptr, &request, &response, nullptr);

    if(response.result().errcode() == 0)
    {
        std::cout << "rpc login success!" << std::endl;
    }
    else
    {
        std::cout << "rpc login error:" << response.result().errmsg() << std::endl;
    }
    return 0;
}