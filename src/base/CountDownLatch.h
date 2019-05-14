#ifndef SERVER_BASE_COUNTDOWNLATCH_H
#define SERVER_BASE_COUNTDOWNLATCH_H

#include"Condition.h"
#include"Mutex.h"

class CountDownLatch{

public:
	CountDownLatch(const CountDownLatch&)=delete;
	void operator=(const CountDownLatch&)=delete;

	explicit CountDownLatch(int count);

	void wait();
	void countDown();
	int getCount()const;

private:
	mutable Mutex mutex_;
	Condition condition_;
	int count_;
};

#endif
