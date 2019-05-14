#ifndef SERVER_BASE_THREAD_H
#define SERVER_BASE_THREAD_H

#include<functional>
#include<string>
#include<pthread.h>
#include<atomic>

class Thread{

public:
	typedef std::function<void()> ThreadFunc;

	Thread(const Thread&)=delete;
	void operator=(const Thread&)=delete;

	explicit Thread(const ThreadFunc&,std::string name=std::string());
	~Thread();

	void start();
	int join();

	bool started()const{return started_;}

	pid_t tid()const{return tid_;}
	std::string name()const{return name_;}

	static int numCreated(){return numCreated_.load();}

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
