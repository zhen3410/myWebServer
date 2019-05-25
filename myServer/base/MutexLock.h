#ifndef SERVER_BASE_MUTEXLOCK_H
#define SERVER_BASE_MUTEXLOCK_H

#include<pthread.h>

class MutexLock{
public:
	MutexLock(const MutexLock&)=delete;
	void operator=(const MutexLock&)=delete;

	MutexLock(){
		pthread_mutex_init(&mutex_,NULL);
	}

	~MutexLock(){
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
	friend class Condition;

	 pthread_mutex_t mutex_;
};

class MutexLockGuard{
public:
	MutexLockGuard(const MutexLockGuard&)=delete;
	void operator=(const MutexLockGuard&)=delete;

	explicit MutexLockGuard(MutexLock& mutex)
		:mutex_(mutex)
	{
		mutex_.lock();
	}

	~MutexLockGuard(){
		mutex_.unlock();
	}

private:
	MutexLock& mutex_;

};

#endif
