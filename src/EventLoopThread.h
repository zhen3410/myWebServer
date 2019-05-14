#ifndef SERVER_SRC_EVENTLOOPINTHREAD_H
#define SERVER_SRC_EVENTLOOPINTHREAD_H

#include"base/Condition.h"
#include"base/Mutex.h"
#include"base/Thread.h"

#include<string>

namespace server{

class EventLoop;

class EventLoopThread{

public:
	//typedef std::function<void(EventLoop*)> ThreadInitCallback;
	
	EventLoopThread(const EventLoopThread&)=delete;
	void operator=(const EventLoopThread&)=delete;

	EventLoopThread(std::string name="EventLoopThread");
	~EventLoopThread();

	EventLoop* startLoop();

private:
	void threadFunc();

	EventLoop* loop_;
	bool existing_;
	Thread thread_;
	Mutex mutex_;
	Condition cond_;
	//ThreadInitCallback callback_;
};

}

#endif
