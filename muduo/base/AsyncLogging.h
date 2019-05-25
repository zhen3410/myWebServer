#ifndef SERVER_ASYNCLOGGING_H
#define SERVER_ASYNCLOGGING_H

namespace server{

class AsyncLogging{

public:
	AsyncLogging(const AsyncLogging&)=delete;
	void operator=(const AsyncLogging&)=delete;

	AsyncLogging(const string& basename,off_t rollSize,int flushInteval=3);
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

private:
	void threadFunc();

	

}

}


#define