#include"EventLoop.h"
#include"EPoller.h"

#include<iostream>
#include<assert.h>

namespace {

__thread EventLoop* t_loopInThisThread=NULL;

}

EventLoop::EventLoop()
	:threadId_(CurrentThread::tid()),
	looping_(false),
	quit_(false);
{
	assert(t_loopInThisThread==NULL);
	t_loopInThisThread=this;
}

EventLoop::~EventLoop(){
	assert(!looping_);
	t_loopInThisThread=NULL;
}

void EventLoop::loop(){
	assert(!looping_);
	assertInLoopThread();
	looping_=true;
	quit_=false;

	while(!quit_){
		poll(&activeChannel_);
		for(int i=0;i<activeChannel_.size();i++){
			auto it=activeChannel_[i];
			it->handleEvent();
		}
	}

	looping_=false;
}

void EventLoop::quit(){
	quit_=true;
	
}
