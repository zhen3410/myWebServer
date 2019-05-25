#ifndef SERVER_BASE_COUNTDOWNLATCH_H
#define SERVER_BASE_COUNTDOWNLATCH_H

class MutexLock;
class Condition;

class CountDownLatch{

public:
	explicit CountDownLatch(int count);
	void wait();
	void countDown();


private:
	MutexLock mutex_;
	Condition condition_;
	int count_;
};


#endif 