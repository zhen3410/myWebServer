#ifndef SERVER_BASE_MUTEX_H
#define SERVER_BASE_MUTEX_H

#include<pthread.h>

class Mutex{

public:
	Mutex(const Mutex&)=delete;
	void operator=(const Mutex&)=delete;

	Mutex(){
		pthread_mutex_init(&mutex_,NULL);
	}
	~Mutex(){
		pthread_mutex_lock(&mutex_);
		pthread_mutex_destroy(&mutex_);
	}

	void lock(){
		pthread_mutex_lock(&mutex_);
	}

	void unlock(){
		pthread_mutex_unlock(&mutex_);
	}

	pthread_mutex_t* get(){
		return &mutex_;
	}

private:
	mutable pthread_mutex_t mutex_;

friend class Condition;	
};

class MutexGuard{

public:
	MutexGuard(const MutexGuard&)=delete;
	void operator=(const MutexGuard&)=delete;

	explicit MutexGuard(Mutex& mutex)
		:mutex_(mutex)
	{
		mutex_.lock();
	}

	~MutexGuard(){
		mutex_.unlock();
	}

private:
	Mutex& mutex_;
};

#endif
