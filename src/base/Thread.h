#ifndef SERVER_BASE_THREAD_H
#define SERVER_BASE_THREAD_H

#include<functional>
#include<string>
#include<phread.h>
#include<atomic>

class Thread{

public:
	typedef std::function<void()> TheadFunc;

	Thread(const Thread&)=delete;
	void operator=(const Thread&)=delete;

	explicit Thread(const ThreadFunc&,string name=string());
	~Thread();

	void start();
	int join();

	bool started()const{return started_;}

	pid_t tid()const{return tid_;}
	string name()const{return name_;}

	static int numCreated(){return numCreated_.load();}

private:
	void setDefaultName();

	bool started_;
	bool joined_;
	pthread_t pthreadId_;
	pid_t tid_;
	ThreadFunc func_;
	string name_;
	//CountDownLatch latch_;

	static std::atomic<int> numCreated_; 
}

#endif