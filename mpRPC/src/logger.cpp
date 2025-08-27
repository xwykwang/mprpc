#include "logger.hpp"
#include <time.h>
#include <iostream>

Logger& Logger::getInstance()
{
    static Logger logger;
    return logger;
}

Logger::Logger()
{
    std::thread writeLogTask([&](){
        for(;;)
        {
            time_t now = time(nullptr);
            tm *nowtm = localtime(&now);

            char file_name[128];
            sprintf(file_name, "%d-%d-%d-log.txt", nowtm->tm_year+1900, nowtm->tm_mon+1, nowtm->tm_mday);

            FILE *pf = fopen(file_name, "a+");
            if(pf == nullptr)
            {
                std::cout<<"logger file :"<<file_name<<"open error!"<<std::endl;
                exit(EXIT_FAILURE);
            }

            std::string msg = lckQue_.pop();
            char time_buf[128] = {0};
            sprintf(time_buf, "%d:%d:%d =>[%s]",
                nowtm->tm_hour, 
                nowtm->tm_min, 
                nowtm->tm_sec, 
                logLevel_ == INFO ? "INFO" : "ERROR");
            msg.insert(0,time_buf);
            msg.append("\n");
            fputs(msg.c_str(), pf);
            fclose(pf);
        }
    });
    writeLogTask.detach();
}

void Logger::setLogLevel(LogLevel lev)
{
    logLevel_ = lev;
}

void Logger::Log(std::string msg)
{
    lckQue_.push(msg);
}