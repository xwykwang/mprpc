#include "mprpcchannel.hpp"
#include "mprpcapplication.hpp"
#include "rpcheader.pb.h"
#include <string>
#include <unistd.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>


void MprpcChannel::CallMethod(const google::protobuf::MethodDescriptor *method,
                    google::protobuf::RpcController *controller,
                    const google::protobuf::Message *request,
                    google::protobuf::Message *response,
                    google::protobuf::Closure *done)
{
    const google::protobuf::ServiceDescriptor* sd = method->service();
    std::string service_name = sd->name();
    std::string method_name = method->name();

    int args_size = 0;
    std::string args_str;
    if(request->SerializeToString(&args_str))
    {
        args_size = args_str.size();
    }
    else
    {
        return;
    }

    mprpc::RpcHeader rpcheader;
    rpcheader.set_service_name(service_name);
    rpcheader.set_method_name(method_name);
    rpcheader.set_args_size(args_size);

    uint32_t header_size = 0;
    std::string header_str;
    if(rpcheader.SerializeToString(&header_str))
    {
        header_size = header_str.size();
    }
    else
    {
        return;
    }

    std::string rpc_send_str;
    rpc_send_str.insert(0, std::string((char*)&header_size, 4));
    rpc_send_str += header_str;
    rpc_send_str += args_str;

    int clientfd = socket(AF_INET, SOCK_STREAM, 0);
    if(clientfd == -1)
    {
        std::cout << "create cliend sockfd failed" << std::endl;
        return;
    }
    std::string ip = MprpcApplication::GetInstance().GetConfig().Load("rpcserverip");
    uint16_t port = atoi(MprpcApplication::GetInstance().GetConfig().Load("rpcserverport").c_str());

    sockaddr_in server_addr;
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    server_addr.sin_addr.s_addr = inet_addr(ip.c_str());

    if(connect(clientfd, (sockaddr*)&server_addr, sizeof(server_addr)) == -1)
    {
        std::cout << "connect to rpc server error" << std::endl;
        close(clientfd);
        return;
    }

    if(send(clientfd, rpc_send_str.c_str(), rpc_send_str.size(), 0) == -1)
    {
        std::cout << "send rpc message error" << std::endl;
        close(clientfd);
        return;
    }

    char buf[1024] = {0};
    int recv_size = 0;
    if((recv_size = recv(clientfd, buf, 1024, 0)) == -1)
    {
        std::cout << "recv error" << std::endl;
        close(clientfd);
        return;
    }

    //std::string response_str(buf, 0, recv_size);
    if(!response->ParseFromArray(buf, recv_size))
    {
        std::cout << "parse response error" <<std::endl;
    }
    close(clientfd);
}