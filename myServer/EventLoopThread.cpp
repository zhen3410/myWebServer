#include"EventLoopThread.h"

#include"EventLoop.h"

#include<assert.h>


//EventLoopThread::EventLoopThread(cosnt EventLoopThread::ThreadInitCallBack& cb,const std::string& name)
EventLoopThread::EventLoopThread(const std::string& name)
    :loop_(NULL),
    //existing_(false),
    thread_(std::bind(&EventLoopThread::threadFunc,this),name),
    mutex_(),
    cond_(mutex_)
    //callback_(cb)
{

}

EventLoopThread::~EventLoopThread(){
    if(loop_!=NULL){
        loop_.quit();
        thread_.join();
    }
}



EventLoop* EventLoopThread::startLoop(){
    assert(!thread_.started());
    thread_.start();
    {
        MutexLockGuard lock(mutex_);
        while(loop_==NULL)
            cond_.wait();
    }
    return loop_;
}


void EventLoopThread::threadFunc(){
    EventLoop loop;
    {
        MutexLockGuard lock(mutex_);
        loop_=&loop;
        cond_.notify();
    }
    loop.loop();
    loop_=NULL;
}