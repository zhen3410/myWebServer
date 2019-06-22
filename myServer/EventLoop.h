#ifndef SERVER_EVENTLOOP_H
#define SERVER_EVENTLOOP_H

#include"base/CurrentThread.h"
#include"EPoller.h"
#include"base/MutexLock.h"

#include<pthread.h>
#include<unistd.h>
#include<sys/syscall.h>
#include<assert.h>
#include<memory>
#include<vector>
#include<functional>


class EventLoop{
public:
	typedef std::function<void()> Functor;
	EventLoop(const EventLoop&)=delete;
	void operator=(const EventLoop&)=delete;

	EventLoop();
	~EventLoop();

	void loop();
	void quit();

	void runInLoop(const Functor&);
	void queueInLoop(const Functor&);

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

	pid_t getThreadId()const{
		return threadId_;
	}

private:
	void handleRead();
	void wakeUp();
	void doPendingFunctors();


	const pid_t threadId_;
	bool looping_;
	bool quit_;
	std::unique_ptr<EPoller> poller_;
	
	typedef std::vector<std::shared_ptr<Channel>> ChannelList;
	ChannelList activeChannel_;

	bool callingFunctors_;
	int wakeupFd_;
	// 这里不能用unique_ptr,否则会造成weak_ptr错误
	std::shared_ptr<Channel> wakeupChannel_;

	MutexLock mutex_;
	std::vector<Functor> pendingFunctors_;
};

#endif
