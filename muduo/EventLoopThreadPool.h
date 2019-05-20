#ifndef SERVER_EVENTLOOPTHREADPOOL_H
#define SERVER_EVENTLOOPTHREADPOOL_H

#include<memory>
#include<vector>

namespace server{

class EventLoop;
class EventLoopThread;

class EventLoopThreadPool{

public:
	EventLoopThreadPool(const EventLoopThreadPool&)=delete;
	void operator=(const EventLoopThreadPool&)=delete;

	EventLoopThreadPool(EventLoop* baseLoop);
	~EventLoopThreadPool();

	void setThreadNum(int numThreads){
		numThreads_=numThreads;
	}

	void start();

	EventLoop* getNextLoop();

private:
	EventLoop* baseLoop_;
	bool started_;
	int numThreads_;
	int next_;
	std::vector<std::unique_ptr<EventLoopThread>> threads_;
	std::vector<EventLoop*> loops_;

};

}

#endif
