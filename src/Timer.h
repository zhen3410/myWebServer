#ifndef SERVER_TIMER_H
#define SERVER_TIMER_H

#include<ctime>
#include<vector>
#include<set>
#include<map>
#include<functional>

namespace server{

	class EventLoop;
	class Channel;

	class TimerQueue{
	public:
		typedef std::function<void()> TimerCallBack;
		TimerQueue(const TimerQueue&)=delete;
		void operator=(const TimerQueue&)=delete;

		int addTimer(const TimerCallBack& cb,time_t when);

		void cancel(int timerId);

	private:
		typedef std::pair<time_t,Timer*> Entry;
		typedef std::set<Entry> TimerList;

		void handleRead();

		std::vector<Entry> getExpired(time_t now);
		void reset(const std::vector<Entry>& expired,time_t now);

		bool insert(Timer* timer);

		EventLoop* loop_;
		const int timerfd_;
		Channel timerfdChannel_;

		TimerList timers_;
	};

	class Timer{
	public:
		typedef std::function<void()> TimerCallBack;
		Timer(TimerCallBack cd,time_t when)
			:callBack_(std::move(cb)),
			expired_(when){}

		void run() const{
			callBack_();
		}

	private:
		const TimerCallBack callBack_;
		time_t expired_;
	}

} // namespace server


#endif