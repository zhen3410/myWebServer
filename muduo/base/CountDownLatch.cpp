#include"CountDownLatch.h"

CountDownLatch::CountDownLatch(int count)
	:mutex_(),
	condition_(mutex_),
	count_(count){}

void CountDownLatch::wait(){
	MutexGuard lock(mutex_);
	while(count_>0){
		condition_.wait();
	}
}

void CountDownLatch::countDown(){
	MutexGuard lock(mutex_);
	--count_;
	if(count_==0){
		condition_.notify();
	}
}

int CountDownLatch::getCount()const{
	MutexGuard lock(mutex_);
	return count_;
}
