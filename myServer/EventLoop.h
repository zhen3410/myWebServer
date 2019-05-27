#ifndef SERVER_EVENTLOOP_H
#define SERVER_EVENTLOOP_H

#include"base/CurrentThread.h"
#include"EPoller.h"

#include<pthread.h>
#include<unistd.h>
#include<sys/syscall.h>
#include<assert.h>
#include<memory>
#include<vector>


class EventLoop{
public:
	EventLoop(const EventLoop&)=delete;
	void operator=(const EventLoop&)=delete;

	EventLoop();
	~EventLoop();

	void loop();
	void quit();

	void assertInLoopThread(){
		assert(isInLoopThread());
	}

	bool isInLoopThread(){
		return threadId_==CurrentThread::tid();
	}

	void removeChannel(std::shared_ptr<Channel> channel){
		poller_->del_event(channel);
	}
	void addChannel(std::shared_ptr<Channel> channel){
		poller_->add_event(channel);
	}
	void updateChannel(std::shared_ptr<Channel> channel){
		poller_->mod_event(channel);
	}

private:
	const pid_t threadId_;
	bool looping_;
	bool quit_;
	unique_ptr<EPoller> poller_;
	
	typedef std::vector<std::shared_ptr<Channel>> ChannelList;
	ChannelList avtiveChannel_;
};

#endif
