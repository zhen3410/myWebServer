#include"EventLoopThread.h"
#include"EventLoop.h"

#include<assert.h>

using namespace server;

EventLoopThread::EventLoopThread(std::string name)
	:loop_(NULL),
	existing_(false),
	thread_(std::bind(&EventLoopThread::threadFunc,this),name),
	mutex_(),
	cond_(mutex_)
{
}

EventLoopThread::~EventLoopThread(){
	existing_=true;
	if(loop_!=NULL){
		loop_->quit();
		thread_.join();
	}
}

EventLoop* EventLoopThread::startLoop(){
	assert(!thread_.started());
	thread_.start();

	EventLoop* loop=NULL;
	{
		MutexGuard lock(mutex_);
		while(loop_==NULL){
			cond_.wait();
		}
		loop=loop_;
	}
	return loop;
}

void EventLoopThread::threadFunc(){

	EventLoop loop;
	{
		MutexGuard lock(mutex_);
		loop_=&loop;
		cond_.notify();
	}

	loop.loop();

	MutexGuard lock(mutex_);
	loop_=NULL;

}
