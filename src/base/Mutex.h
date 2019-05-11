#ifndef SERVER_BASE_MUTEX_H
#define SERVER_BASE_MUTEX_H

#include<pthread.h>

class Mutex{

public:
	Mutex(const Mutex&)=delete;
	void operator=(const Mutex&)=delete;

	Mutex(){
		pthead_mutex_init(&mutex_,NULL);
	}
	~Mutex(){
		pthread_mutex_lock(&mutex);
		pthread_mutex_destroy(&mutex);
	}

	void lock(){
		pthread_mutex_lock();
	}

	void unlock(){
		pthread_mutex_unlock();
	}

	pthread_mutex_t* get(){
		return &mutex_;
	}

private:
	mutable pthread_mutex_t mutex_; 
}

class MutexGuard{

public:
	Mutex(const Mutex&)=delete;
	void operator=(const Mutex&)=delete;

	explicit MutexGuard(Mutex& mutex)
		:mutex_(mutex)
	{
		mutex_.lock();
	}

	~MutexGuard(){
		mutex_.unlock();
	}

private:
	Mutex mutex_;
}

#endif