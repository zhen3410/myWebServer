#include"EventLoop.h"
#include"EPoller.h"
#include"Channel.h"

#include<iostream>
#include<assert.h>
#include<sys/eventfd.h>
#include<signal.h>

namespace {

__thread EventLoop* t_loopInThisThread=NULL;

int creatEventFd(){
	int evFd=eventfd(0,EFD_NONBLOCK|EFD_CLOEXEC);
	assert(evFd>=0);
	return evFd;
}

}

class IgnoreSigPipe{
public:
	IgnoreSigPipe(){
		signal(SIGPIPE,SIG_IGN);
	}
};

IgnoreSigPipe initObj;

EventLoop::EventLoop()
	:threadId_(CurrentThread::tid()),
	looping_(false),
	quit_(false),
	poller_(new EPoller(*this)),
	callingFunctors_(false),
	wakeupFd_(creatEventFd()),
	wakeupChannel_(new Channel(*this,wakeupFd_)),
	mutex_()
{
	assert(t_loopInThisThread==NULL);
	t_loopInThisThread=this;
	wakeupChannel_->setReadCallBack(std::bind(&EventLoop::handleRead,this));
	wakeupChannel_->enableReading();
	//std::cout<<"EventLoop::EventLoop() threadId = "<<threadId_<<" , wakeupFd = "<<wakeupFd_<<" construct succeed."<<std::endl;
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
		poller_->poll(activeChannel_);
		for(int i=0;i<activeChannel_.size();i++){
			activeChannel_[i]->handleEvent();
		}
		doPendingFunctors();
	}

	looping_=false;
}

void EventLoop::quit(){
	quit_=true;
	if(!isInLoopThread()){
		wakeUp();
	}
}


void EventLoop::runInLoop(const Functor& cb){
	if(isInLoopThread()){
		cb();
	}else{
		queueInLoop(cb);
	}
}

void EventLoop::queueInLoop(const Functor& cb){
	{
		MutexLockGuard lock(mutex_);
		pendingFunctors_.push_back(cb);
	}
	if(!isInLoopThread()||callingFunctors_){
		wakeUp();
	}
}

void EventLoop::handleRead(){
	uint64_t one=1;
	ssize_t n=read(wakeupFd_,&one,sizeof one);
	assert(n==sizeof one);
}

void EventLoop::wakeUp(){
	uint64_t one=1;
	ssize_t n=write(wakeupFd_,&one,sizeof one);
	assert(n==sizeof one);
}

void EventLoop::doPendingFunctors(){
	assert(!callingFunctors_);
	std::vector<Functor> functors;
	callingFunctors_=true;

	{
		MutexLockGuard lock(mutex_);
		functors.swap(pendingFunctors_);
	}

	for(auto it=functors.begin();it!=functors.end();it++)
		(*it)();

	callingFunctors_=false;
}
