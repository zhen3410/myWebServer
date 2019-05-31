#ifndef SERVER_EVENTLOOPTHREAD_H
#define SERVER_EVENTLOOPTHREAD_H


#include"base/MutexLock.h"
#include"base/Thread.h"

#include<functional>
#include<string>

class EventLoop;
class MutexLock;

class EventLoopThread
{
public:
    //typedef std::function<void(EventLoop&)> ThreadInitCallBack;
    EventLoopThread(const EventLoopThread&)=delete;
    void operator=(const EventLoopThread&)=delete;

    //EventLoopThread(const ThreadInitCallBack& cb=ThreadInitCallBack(),const std::string& name=std::String());
    EventLoopThread(const std::string& name=std::string());
    ~EventLoopThread();
    EventLoop* startLoop();

private:
    void threadFunc();

    EventLoop* loop_;
    //bool existing_;
    Thread thread_;
    MutexLock mutex_;
    Condition cond_;
    //ThreadInitCallBack callback_;

};



#endif