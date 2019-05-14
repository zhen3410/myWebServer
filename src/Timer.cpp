#include"Timer.h"
#include"EventLoop.h"

#include<sys/timerfd.h>
#include<iostream>
#include<unistd.h>
#include<memory.h>
#include<assert.h>
#include<iterator>

namespace{

int createTimerfd(){
	int timerfd=::timerfd_create(CLOCK_MONOTONIC,TFD_NONBLOCK|TFD_CLOEXEC);
	if(timerfd<0){
		std::cerr<<"createTimerfd() failed"<<std::endl;
	}
	return timerfd;
}

void readTimerfd(int timerfd,Timestamp now){
	uint64_t exp;
	ssize_t n=::read(timerfd,&exp,sizeof exp);
	std::cout<<"TimerQueue::handleRead() "<<exp<<" at "<<now.get()<<std::endl;
	if(n!=sizeof exp){
		std::cerr<<"TimerQueue::handleRead() reads "<<n<<" bytes instead of 8 bytes"<<std::endl;
	}
}

void resetTimerfd(int timerfd,Timestamp expiration){
	struct itimerspec newValue;
	memset(&newValue,0,sizeof newValue);
	int64_t microSeconds=expiration.get()-Timestamp::now().get();
	struct timespec ts;
	ts.tv_sec=static_cast<time_t>(microSeconds/Timestamp::kMicroSecondsPreSecond);
	ts.tv_nsec=static_cast<time_t>(microSeconds%Timestamp::kMicroSecondsPreSecond*1000);
	int ret=::timerfd_settime(timerfd,0,&newValue,NULL);
	if(ret){
		std::cerr<<"resetTimerfd():timerfd_settime() failed"<<std::endl;
	}
}

}

using namespace server;

void Timer::restart(Timestamp now){
	if(repeat_){
		expiration_=addTime(now,interval_);
	}else{
		expiration_=new Timestamp();
	}
}

TimerQueue::TimerQueue(EventLoop* loop)
	:loop_(loop),
	timerfd_(createTimerfd()),
	timerfdChannel_(loop_,timerfd_),
	timers_(),
	callingExpiredTimers_(false)
{
	timerfdChannel_.setReadCallback(std::bind(&TimerQueue::handleRead,this));
	timerfdChannel_.enableReading();
}

TimerQueue::~TimerQueue(){
	timerfdChannel_.disableAll();
	timerfdChannel_.remove();
	::close(timerfd_);

	for(const Entry& timer:timers_){
		delete timer.second;
	}
}


std::vector<server::TimerQueue::Entry> TimerQueue::getExpired(Timestamp now){
	std::vector<Entry> expired;
	Entry sentry={now,reinterpret_cast<Timer*>(UINTPTR_MAX)};
	TimerList::iterator it=timers_.lower_bound(sentry);
	assert(it==timers_.end()||now<it->first);
	std::copy(timers_.begin(),it,back_inserter(expired));
	timers_.erase(timers_.begin(),it);
	return expired;	
}

TimerId TimerQueue::addTimer(const server::TimerQueue::TimerCallBack& cb,
	Timestamp when,double interval){
	Timer* timer=new Timer(cb,when,interval);
	loop_->runInLoop(std::bind(&TimerQueue::addTimerInLoop,this,timer));
	return TimerId(timer,timer->sequence());
}

void TimerQueue::addTimerInLoop(Timer* timer){
	loop_->assertInLoopThread();
	bool earliestChanged=insert(timer);
	if(earliestChanged){
		resetTimerfd(timerfd_,timer->expiration());
	}
}

void TimerQueue::handleRead(){
	loop_->assertInLoopThread();
	Timestamp now(Timestamp::now());
	readTimerfd(timerfd_,now);

	std::vector<Entry> expired=getExpired(now);

	callingExpiredTimers_=true;
	cancelingTimers_.clear();

	for(const Entry& it: expired){
		it.second->run();
	}
	callingExpiredTimers_=false;

	reset(expired,now);
}

void TimerQueue::reset(const std::vector<Entry>& expired,Timestamp now){
	Timestamp nextExpire;

	for(const Entry& it:expired){
		ActiveTimer timer(it.second,it.second->sequence());
		// 如果设置了重复标志且没有在取消队列中，则重新加入到队列中
		if(it.second->repeat()&&cancelingTimers_.find(timer)==cancelingTimers_.end()){
			it.second->restart(now);
			insert(it.second);
		}else{
			delete it.second;
		}
	}

	// 将timerfd的时间设置为当前时间队列timers_的最早时间
	if(!timers_.empty()){
		nextExpire=timers_.begin()->second->expiration();
	}
	if(nextExpire.get()>0){
		resetTimerfd(timerfd_,nextExpire);
	}
}

bool TimerQueue::insert(Timer* timer){
	loop_->assertInLoopThread();
	assert(timers_.size()==activeTimers_.size());
	bool earliestChanged=false;
	Timestamp when=timer->expiration();
	TimerList::iterator it=timers_.begin();
	if(it==timers_.end()||when<it->first){
		earliestChanged=true;
	}
	{
		std::pair<TimerList::iterator,bool> result
		=timers_.insert(Entry(when,timer));
		assert(result.second);
	}
	{
		std::pair<ActiveTimerSet::iterator,bool> result
		=activeTimers_.insert(ActiveTimer(timer,timer->sequence()));
		assert(result.second);
	}
	assert(timers_.size()==activeTimers_.size());
	return earliestChanged;
}