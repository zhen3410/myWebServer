#ifndef SERVER_BASE_THREAD_H
#define SERVER_BASE_THREAD_H

#include"CountDownLatch.h"

#include<pthread.h>
#include<functional>
#include<string>
#include<atomic>

class Thread{

public:
	Thread(const Thread&)=delete;
	void operator=(const Thread&)=delete;

	typedef std::function<void()> ThreadFunc;
	explicit Thread(const ThreadFunc&,const std::string name=std::string());
	~Thread();

	void start();
	int join();

	bool started()const{return started_;}

	pid_t tid()const{return tid_;}
	const std::string& name()const{return name_;}

private:
	void setDefaultName();

	bool started_;
	bool joined_;
	pthread_t pthreadId_;
	pid_t tid_;
	ThreadFunc func_;
	std::string name_;
	CountDownLatch latch_;

	static std::atomic<int> numCreated_;
};

#endif
