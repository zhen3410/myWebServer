#ifndef SERVER_SRC_EVENTLOOP_H
#define SERVER_SRC_EVENTLOOP_H

#include"base/Condition.h"
#include"base/Mutex.h"
#include"base/Thread.h"

namespace server{

class EventLoop;

class EventLoopThread{

public:
	//typedef std::function<void(EventLoop*)> ThreadInitCallback;

	EventLoopThread(string name=string());
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
}

}

#endif