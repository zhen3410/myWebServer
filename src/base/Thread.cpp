#include"Thread.h"

#include<iostream>

namespace {
	struct FuncArg{

	};
}

std::atomic<int> numCreated_(0);

Threaed::Thread(const ThreadFunc& func,string name)
	:started_(false),
	joined_(false),
	pthreadId_(0),
	tid_(0),
	func_(func),
	name_(name),
{
	setDefaultName();
}

void Thread::setDefaultName(){
	int num=numCreated_.fetch_add(1).load();
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

void Thread::join(){
	assert(started_);
	assert(!joined_);
	joined_=true;
	return pthread_join(pthreadId_,NULL);
}


