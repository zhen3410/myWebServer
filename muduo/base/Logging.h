#ifndef SERVER_BASE_LOGGING_H
#define SERVER_BASE_LOGGING_H


#include"LogStream.h"
#include"Timestamp.h"


namespace server{

class Logger{

public:
    Logger(const char* filename,int line);
    ~Logger();

    LogStream& stream(){
        return impl_.stream_;
    }



private:
    class Impl{
	    public:
        Impl(const char* filename,int line);
        void formatTime();

        Timestamp time_;
        LogStream stream_;
        int line_;
        std::string baseName_;
    };
    Impl impl_;
};

#define LOG Logger(_FILE_,_LINE_).stream();

}


#endif
