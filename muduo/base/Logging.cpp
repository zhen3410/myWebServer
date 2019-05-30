#include"Logging.h"

//#include"CurrentThread.h"
#include"AsyncLogging.h"
#include"Timestamp.h"

#include<pthread.h>
#include<sys/syscall.h>
#include<unistd.h>

namespace server{

namespace {
    pthread_once_t once_control_=PTHREAD_ONCE_INIT;
    AsyncLogging *AsyncLogger;

    void once_init(){
        AsyncLogger=new AsyncLogging("Zhen_server",100*1024*1024);
        AsyncLogger->start();
    }
    void output(const char* msg,int len){
        pthread_once(&once_control_,once_init);
        AsyncLogger->append(msg,len);
    }
    
}

Logger::Impl::Impl(const char* filename,int line)
    :time_(Timestamp::now()),
    stream_(),
    line_(),
    baseName_(filename)
{
    formatTime();
    stream_<<syscall(SYS_gettid);
}

void Logger::Impl::formatTime(){
    stream_<<time_.get();
}

Logger::Logger(const char* filename,int line)
    :impl_(filename,line)
{

}

Logger::~Logger(){
    impl_.stream_<<"--"<<impl_.baseName_<<":"<<impl_.line_<<'\n';
    const LogStream::Buffer& buf(stream().buffer());
    output(buf.data(),buf.length());
}



}
