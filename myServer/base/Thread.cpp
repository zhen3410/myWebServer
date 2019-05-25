#include"Thread.h"
#include"CurrentThread.h"

#include<iostream>
#include<assert.h>
#include<unistd.h>
#include<sys/syscall.h>
#include<sys/prctl.h>

namespace CurrentThread{
	__thread int t_cachedTid=0;
	__thread char t_tidString[32];
	__thread int t_tidStringLength=6;
	__thread const char* t_threadName="default";
}

pid_t gettid(){
	return static_cast<pid_t>(::syscall(SYS_gettid));
}

void CurrentThread::cacheTid(){
	if(t_cachedTid==0){
		t_cachedTid=gettid();
		t_tidStringLength=snprintf(t_tidString,sizeof t_tidString,"%5d ",t_cachedTid);
	}
}

namespace {
	struct FuncAndArg{
		Thread::ThreadFunc func_;
		std::string name_;
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
			CurrentThread::t_threadName=name_.empty()?"Thread":name_.c_str();
			prctl(PR_SET_NAME,CurrentThread::t_threadName);
			func_();
			CurrentThread::t_threadName="finished";
		}
	};

	void* startThread(void* obj){
		FuncAndArg* data=static_cast<FuncAndArg*>(obj);
		data->runInThread();
		delete data;
		return NULL;
	}
}

std::atomic<int> Thread::numCreated_(0);

Thread::Thread(const Thread::ThreadFunc& func,std::string name)
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

Thread::~Thread(){
	if(started_&&!joined_)
		pthread_detach(pthreadId_);
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
	FuncAndArg* data=new FuncAndArg(func_,name_,&tid_,&latch_);
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


