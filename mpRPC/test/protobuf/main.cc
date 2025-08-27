#include "test.pb.h"
#include <iostream>
#include <string>
using namespace fixbug;

int main()
{
    LoginRequest req;
    req.set_name("zhangsan");
    req.set_pwd("123456");

    std::string str;
    if(req.SerializeToString(&str))
    {
        std::cout<<str.c_str()<<std::endl;
    }

    LoginRequest req1;
    if(req1.ParseFromString(str))
    {
        std::cout<< req1.name()<<std::endl;
        std::cout<< req1.pwd() <<std::endl;
    }
    return 0;
}