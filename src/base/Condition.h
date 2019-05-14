#ifndef SERVER_BASE_CONDITION_H
#define SERVER_BASE_CONDITION_H

#include"Mutex.h"

#include<pthread.h>

class Condition{

public:
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


private:
	Mutex& mutex_;
	pthread_cond_t pcond_;
}

#endif