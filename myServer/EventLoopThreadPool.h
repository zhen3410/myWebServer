#ifndef SERVER_EVENTLOOPTHREADPOOL_H
#define SERVER_EVENTLOOPTHREADPOOL_H

#include<vector>
#include<memory>
#include<string>

class EventLoop;
class EventLoopThread;

class EventLoopThreadPool
{
public:
    EventLoopThreadPool(const EventLoopThreadPool&)=delete;
    void operator=(const EventLoopThreadPool&)=delete;

    EventLoopThreadPool(EventLoop& loop,const std::string& name);
    ~EventLoopThreadPool();

    void start();

    void setThreadNum(int num){
        threadNum_=num;
    }

    EventLoop* getNextLoop();

private:
    /* data */
    EventLoop& baseLoop_;
    int threadNum_;
    const std::string name_;
    int next_;

    std::vector<std::unique_ptr<EventLoopThread>> threads_;
    std::vector<EventLoop*> loops_;
};

#endif