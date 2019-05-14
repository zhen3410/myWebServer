#include"Thread.h"

#include<iostream>
#include<assert.h>

namespace {
	struct FuncAndArg{
		Thread::ThreadFunc func_;
		std::string name;
		pid_t* tid_;
		CountDownLatch* latch_;

		FuncAndArg(const Thread::ThreadFunc& func,const std::string& name,pid_t* tid,CountDownLatch* latch)
			:func_(func),
			name_(name),
			tid_(tid),
			latch_(latch){}

		void runInThread(){
			*tid_=pthread_self();
			tid_=NULL;
			latch_->countDown();
			latch_=NULL;
			func_();
		}
	};

	void* startThread(void* obj){
		FuncAndArg* data=static_cast<FuncAndArg*>(obj);
		data->runInThread();
		delete data;
		return NULL;
	}
}

std::atomic<int> numCreated_(0);

Thread::Thread(const ThreadFunc& func,std::string name)
	:started_(false),
	joined_(false),
	pthreadId_(0),
	tid_(0),
	func_(func),
	name_(name),
	latch_(1)
{
	setDefaultName();
}

void Thread::setDefaultName(){
	int num=++numCreated_;
	if(name_.empty()){
		name_="Thread"+std::to_string(num);
	}
}

void Thread::start(){
	assert(!started_);
	started_=true;
	FuncAndArg* data=new FuncAndArg(func_,name_,&tid_,&latch_)；
	int ret=pthread_create(&pthreadId_,NULL,&startThread,data);
	if(ret){
		started_=false;
		std::cerr<<"Failed in pthread_create"<<std::endl;
	}else{
		latch_.wait();
	}
}

int Thread::join(){
	assert(started_);
	assert(!joined_);
	joined_=true;
	return pthread_join(pthreadId_,NULL);
}


