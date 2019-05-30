#ifndef SERVER_BASE_CONDITION_H
#define SERVER_BASE_CONDITION_H

#include"Mutex.h"

#include<pthread.h>
#include<errno.h>
#include<stdint.h>

class Condition{

public:
	Condition(const Condition&)=delete;
	void operator=(const Condition&)=delete;

	explicit Condition(Mutex& mutex)
		:mutex_(mutex)
	{
		pthread_cond_init(&pcond_,NULL);
	}
	~Condition(){
		pthread_cond_destroy(&pcond_);
	}
	void wait(){
		pthread_cond_wait(&pcond_,mutex_.get());
	}
	void notify(){
		pthread_cond_signal(&pcond_);
	}
	bool waitForSeconds(int seconds){
		struct timespec abstime;
		clock_gettime(CLOCK_REALTIME,&abstime);

		const int64_t kNanoSecondsPerSecond=1000000000;
		int64_t nanoseconds=static_cast<int64_t>(seconds*kNanoSecondsPerSecond);

		abstime.tv_sec+=static_cast<time_t>((abstime.tv_nsec+nanoseconds)/kNanoSecondsPerSecond);
		abstime.tv_nsec=static_cast<long>((abstime.tv_nsec+nanoseconds)%kNanoSecondsPerSecond);

		return ETIMEDOUT==pthread_cond_timedwait(&pcond_,mutex_.get(),&abstime);
	}


private:
	Mutex& mutex_;
	pthread_cond_t pcond_;
};

#endif
