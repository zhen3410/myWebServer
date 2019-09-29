#ifndef SERVER_ASYNCLOGGING_H
#define SERVER_ASYNCLOGGING_H

#include"Thread.h"
#include"CountDownLatch.h"
#include"LogStream.h"

#include<atomic>
#include<string>
#include<vector>
#include<memory>

namespace server{

class AsyncLogging{

public:
	AsyncLogging(const AsyncLogging&)=delete;
	void operator=(const AsyncLogging&)=delete;

	AsyncLogging(const std::string& basename,off_t rollSize,int flushInteval=3);
	~AsyncLogging(){
		if(running_){
			stop();
		}
	}

	void append(const char* logline,int len);

	void start(){
		running_=true;
		thread_.start();
		latch_.wait();
	}

	void  stop(){
		running_=false;
		cond_.notify();
		thread_.join();
	}

	void threadFunc();
private:
	typedef FixedBuffer<kLargeBuffer> Buffer;
	typedef std::unique_ptr<Buffer> BufferPtr;
	typedef std::vector<BufferPtr> BufferVector;


	const int flushInterval_;
	const std::string basename_;
	const off_t rollSize_;

	Thread thread_;
	CountDownLatch latch_;
	std::atomic<bool> running_;
	MutexLock mutex_;
	Condition cond_;

	BufferPtr currentBuffer_;
	BufferPtr nextBuffer_;
	BufferVector buffers_;

};

}


#endif
