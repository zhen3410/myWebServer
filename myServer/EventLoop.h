#ifndef SERVER_EVENTLOOP_H
#define SERVER_EVENTLOOP_H

#include"CurrentThead.h"

#include<pthread.h>
#include<unistd.h>
#include<sys/syscall.h>

class EventLoop{
public:
	EventLoop(const EventLoop&)=delete;
	void operator=(const EventLoop&)=delete;

	EventLoop();
	~EventLoop();

	void loop();

	void assertInLoopThread(){
		assert(isInLoopThread());
	}

	bool isInLoopThread(){
		return threadId_==CurrentThead::tid();
	}

private:
	const pid_t threadId_;
	bool looping_;
}

#endif