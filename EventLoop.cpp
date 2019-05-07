#include"EventLoop.h"
#include"Poller"

#include<iostream>
#include<assert.h>
#include<poll.h>

using namespace server;

namespace{

__thread EventLoop* t_loopInThisThread=0;

const int kPollTimeMs=10000;
}

EventLoop::EventLoop()
    :looping_(false),
    threadId_(pthread_self())
{
    if(t_loopInThisThread){
        std::cerr<<"another thread exist"<<std::endl;
    }else{
        t_loopInThisThread=this;
    }
}

EventLoop::~EventLoop(){
    assert(!looping_);
    t_loopInThisThread=NULL;
}

EventLoop* EventLoop::getEventLoopOfCurrentThread(){
    return t_loopInThisThread;
}

void EventLoop::loop(){
    assert(!looping_);
    assertInLoopThread();
    looping_=true;
    quit_=false;

    while(!quit_){
        activeChannels_.clear();
        poller_->poll(kPollTimeMs,&activeChannels_);
        for(ChannelList::iterator it=activeChannels_.begin();it!=activeChannels_.end();++it){
            (*it)->handleEvent();
        }
    }

    //::poll(NULL,0,5*1000);

    std::cout<<"EventLoop"<<threadId_<<"stop looping"<<std::endl;
    looping_=false;
}

void EventLoop::quit(){
    quit_=true;
}

void EventLoop::updateChannel(Channel* channel){
    assert(channel->ownerLoop==this);
    assertInLoopThread();
    poller_->updateChannel(channel);
}

void EventLoop::abortNotInLoopThread(){
    std::cerr<<"EventLoop::abortNotInLoopThread() EventLoop was created in threadId_ = " << threadId_ << " , current thread id = " <<pthread_self()<<std::endl;
}
