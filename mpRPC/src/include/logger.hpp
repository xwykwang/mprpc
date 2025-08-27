#pragma once
#include "lockqueue.hpp"
#include <string>

enum LogLevel
{
    INFO,
    ERROR,
};


class Logger
{
public:
    static Logger& getInstance();
    void Log(std::string msg);
    
    void setLogLevel(LogLevel level);
private:
    Logger();
    int logLevel_;
    LockQueue<std::string> lckQue_;
    Logger(const Logger&) = delete;
    Logger(Logger&&) = delete;
};