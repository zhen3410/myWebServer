#include"Timer.h"

std::vector<server::TimerQueue::Entry> server::TimerQueue::getExpired(time_t now){
	std::vector<Entry> expired;
	Entry sentry={now,reinterpret_cast<Timer*>(UINTPTR_MAX)};
	TimerList::iterator it=timers_.lower_bound(sentry);
	assert(it==timers_.end()||now<it->first);
	std::copy(timers_.begin(),it,back_inserter(exired));
	timers_.erase(timers_.begin(),it);
	return expired;	
}

int server::TimerQueue::addTimer(const server::TimerQueue::TimerCallBack& cb
	time_t when){
	TimerList.insert({when,new Timer(cb,when)});
}