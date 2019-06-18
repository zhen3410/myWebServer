#include"EventLoopThreadPool.h"
#include"EventLoopThread.h"
#include"EventLoop.h"

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
	baseLoop_.assertInLoopThread();
    for(int i=0;i<threadNum_;i++){
        std::string threadName=name_+std::to_string(i);
        EventLoopThread* t=(new EventLoopThread(threadName));
        threads_.push_back(std::unique_ptr<EventLoopThread>(t));
        loops_.push_back(t->startLoop());
    }
}

EventLoop* EventLoopThreadPool::getNextLoop(){
	baseLoop_.assertInLoopThread();
    EventLoop* loop=&baseLoop_;
    //std::cout<<"EventLoopThreadPool::getNextLoop() begin select Loop , pool size = "<<loops_.size()<<" , next = "<<next_<<std::endl;
    //std::cout<<"Threads nums = "<<threads_.size()<<std::endl;
    if(threadNum_>0){
        loop=loops_[next_];
    //std::cout<<"EventLoopThreadPool::getNextLoop() select EventLoop name = "<<threads_[next_]->name()<<std::endl;
    next_++;
        if(next_>=threadNum_)next_=0;
    }
    return loop;
}
