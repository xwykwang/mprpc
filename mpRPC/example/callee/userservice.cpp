#include <iostream>
#include <string>
#include "user.pb.h"
#include "mprpcapplication.hpp"
#include "rpcprovider.hpp"


using namespace fixbug;

class UserService : public UserServiceRpc
{
public:
    bool Login(std::string name, std::string pwd)
    {
        std::cout<< name << std::endl;
        std::cout<< pwd << std::endl;
        return true;
    }

    void Login(::google::protobuf::RpcController* controller,
                       const ::fixbug::LoginRequest* request,
                       ::fixbug::LoginResponse* response,
                       ::google::protobuf::Closure* done)
    {
        std::string name = request->name();
        std::string pwd = request->pwd();

        bool res = Login(name, pwd);

        response->set_success(res);
        ResultCode * rc = response->mutable_result();
        rc->set_errcode(0);
        rc->set_errmsg("null");

        done->Run();
    }

private:
};

int main(int argc, char **argv)
{
    MprpcApplication::Init(argc, argv);

    RpcProvider provider;
    provider.NotifyService(new UserService);
    provider.Run();

    return 0;
}