#pragma once
#include <google/protobuf/service.h>
#include <muduo/net/TcpServer.h>
#include <muduo/net/EventLoop.h>
#include <muduo/net/InetAddress.h>
#include <muduo/net/TcpConnection.h>
#include <functional>
#include <unordered_map>
#include <google/protobuf/descriptor.h>


class RpcProvider
{
public:
    void NotifyService(google::protobuf::Service *Service);
    void Run();

private:
    muduo::net::EventLoop eventLoop_;
    void OnConnection(const muduo::net::TcpConnectionPtr &);
    void OnMessage(const muduo::net::TcpConnectionPtr&, muduo::net::Buffer*, muduo::Timestamp);
    void SendRpcResponse(const muduo::net::TcpConnectionPtr&, google::protobuf::Message*);

    struct ServiceInfo
    {
        google::protobuf::Service *mservice_;
        std::unordered_map<std::string, const google::protobuf::MethodDescriptor*> methodMap_;
    };
    std::unordered_map<std::string, ServiceInfo> serviceMap_;
};