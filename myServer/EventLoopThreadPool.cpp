#include"EventLoopThreadPool.h"
#include"EventLoopThread.h"

#include<iostream>

EventLoopThreadPool::EventLoopThreadPool(EventLoop& loop,const std::string& name)
    :baseLoop_(loop),
    threadNum_(0),
    name_(name),
    next_(0)
{

}

EventLoopThreadPool::~EventLoopThreadPool(){

}

void EventLoopThreadPool::start(){
    for(int i=0;i<threadNum_;i++){
        std::string threadName=name_+std::to_string(i);
        EventLoopThread* t=(new EventLoopThread(threadName));
        EventLoop* loop=t->startLoop();
        threads_.push_back(std::unique_ptr<EventLoopThread>(t));
        loops_.push_back(loop);
    }
}

EventLoop* EventLoopThreadPool::getNextLoop(){
    EventLoop* loop=&baseLoop_;
    std::cout<<"EventLoopThreadPool::getNextLoop() begin select Loop"<<std::endl;
    if(threadNum_>0){
        loop=loops_[next_++];
    std::cout<<"EventLoopThreadPool::getNextLoop() select EventLoop name = "<<threads_[next_-1]->name()<<std::endl;
        if(next_>=threadNum_)next_=0;
    }
    return loop;
}
