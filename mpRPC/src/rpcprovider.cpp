#include "rpcprovider.hpp"
#include "mprpcapplication.hpp"
#include "rpcheader.pb.h"
#include <muduo/net/TcpConnection.h>


void RpcProvider::NotifyService(google::protobuf::Service *service)
{
    ServiceInfo info;
    const google::protobuf::ServiceDescriptor *dsp = service->GetDescriptor();
    std::string service_name = dsp->name();
    int methodCnt = dsp->method_count();
    std::cout<<"service name :"<<service_name<<std::endl;
    for(int i=0; i<methodCnt; i++)
    {
        const google::protobuf::MethodDescriptor* mthDsp = dsp->method(i);
        std::string method_name = mthDsp->name();
        std::cout<<"method_name :"<<method_name<<std::endl;
        info.methodMap_.insert({method_name, mthDsp}); 
    }
    info.mservice_ = service;
    serviceMap_.insert({service_name, info});
}

void RpcProvider::Run()
{
    auto cnf = MprpcApplication::GetInstance().GetConfig();
    std::string ip = cnf.Load("rpcserverip");
    uint16_t port = atoi(cnf.Load("rpcserverport").c_str());
    muduo::net::InetAddress address(ip, port);

    muduo::net::TcpServer server(&eventLoop_, address, "rpcserver");
    server.setConnectionCallback(std::bind(&RpcProvider::OnConnection, this, std::placeholders::_1));
    server.setMessageCallback(std::bind(&RpcProvider::OnMessage, this, 
        std::placeholders::_1, std::placeholders::_2, std::placeholders::_3));

    server.setThreadNum(4);
    std::cout<< "start RpcServer in ip:"<< ip << " port:" << port << std::endl;
    server.start();
    std::cout<< "start Loop"<<std::endl;
    eventLoop_.loop();
}

void RpcProvider::OnConnection(const muduo::net::TcpConnectionPtr &conn)
{
    if(!conn->connected())
    {
        conn->shutdown();
    }
}

void RpcProvider::OnMessage(const muduo::net::TcpConnectionPtr &conn, muduo::net::Buffer *buff, muduo::Timestamp time)
{
    std::string recv_buf = buff->retrieveAllAsString();
    uint32_t header_size = 0;
    recv_buf.copy((char*)&header_size, 4, 0);

    std::string head_str = recv_buf.substr(4, header_size);
    mprpc::RpcHeader rpcHeader;
    std::string service_name;
    std::string method_name;
    uint32_t args_size;
    if(!rpcHeader.ParseFromString(head_str))
    {
        std::cout<< "parse rpc head error" <<std::endl;
        return;
    }
    service_name = rpcHeader.service_name();
    method_name = rpcHeader.method_name();
    args_size = rpcHeader.args_size();

    std::string args_str = recv_buf.substr(4 + header_size, args_size);
    auto it = serviceMap_.find(service_name);
    if(it ==  serviceMap_.end())
    {
        std::cout << "service invalid" << std::endl;
        return;
    }

    auto mit = it->second.methodMap_.find(method_name);
    if(mit == it->second.methodMap_.end())
    {
        std::cout << "method invalid" << std::endl;
        return;
    }

    google::protobuf::Service *service = it->second.mservice_;
    const google::protobuf::MethodDescriptor *method = mit->second;

    google::protobuf::Message *request = service->GetRequestPrototype(method).New();
    google::protobuf::Message *response = service->GetResponsePrototype(method).New();
    if(!request->ParseFromString(args_str))
    {
        std::cout<< "args invalid" << std::endl;
        return;
    }

    google::protobuf::Closure *done = google::protobuf::NewCallback<RpcProvider, \
    const muduo::net::TcpConnectionPtr&, google::protobuf::Message*>\
    (this, &RpcProvider::SendRpcResponse, conn, response);

    service->CallMethod(method, nullptr, request, response, done);
}

void RpcProvider::SendRpcResponse(const muduo::net::TcpConnectionPtr &conn, google::protobuf::Message *response)
{
    std::string response_str;
    if(response->SerializeToString(&response_str))
    {
        conn->send(response_str);
        
    }
    else
    {
        std::cout << "response serialize failed" << std::endl;
    }
    conn->shutdown();
}