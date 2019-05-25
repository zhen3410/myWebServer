#include"EventLoop.h"
#include"poll.h"

namespace {

__thread EventLoop* t_loopInThisThread=NULL;

}

EventLoop::EventLoop()
	:threadId_(CurrentThread::tid()),
	looping_(false);
{
	if(t_loopInThisThread){
		std::cerr<<"EventLoop::EventLoop() another EventLoop in this thread"<<std::endl;
	}else{
		t_loopInThisThread=this;
	}
}

EventLoop::~EventLoop(){
	assert(!looping_);
	t_loopInThisThread=NULL;
}

void EventLoop::loop(){
	assertInLoopThread();
	looping_=true;

	poll(NULL,0,5*1000);

	looping_=false;
}