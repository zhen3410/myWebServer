#include"Thread.h"

#include<iostream>
#include<assert.h>

namespace {
	struct FuncArg{

	};
}

std::atomic<int> numCreated_(0);

Thread::Thread(const ThreadFunc& func,std::string name)
	:started_(false),
	joined_(false),
	pthreadId_(0),
	tid_(0),
	func_(func),
	name_(name)
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
	int ret=pthread_create(&pthreadId_,NULL,func_,NULL);
	if(ret){
		started_=false;
		std::cerr<<"Failed in pthread_create"<<std::endl;
	}else{

	}
}

int Thread::join(){
	assert(started_);
	assert(!joined_);
	joined_=true;
	return pthread_join(pthreadId_,NULL);
}


