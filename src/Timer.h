#ifndef SERVER_TIMER_H
#define SERVER_TIMER_H

#include"./base/Timestamp.h"
#include"channel.h"

#include<vector>
#include<set>
#include<map>
#include<functional>
#include<atomic>

namespace server{

	class EventLoop;

	class Timer{
	public:
		typedef std::function<void()> TimerCallBack;
		Timer(TimerCallBack cb,Timestamp when,double interval)
			:callBack_(std::move(cb)),
			expiration_(when),
			interval_(interval),
			repeat_(interval>0.0),
			sequence_(++numCreated_){}

		void run() const{
			callBack_();
		}
		void restart(Timestamp now);
		int64_t sequence()const{
			return sequence_;
		}
		Timestamp expiration()const{
			return expiration_;
		}
		bool repeat(){
			return repeat_;
		}

	private:
		const TimerCallBack callBack_;
		Timestamp expiration_;
		const double interval_;
		const bool repeat_;
		const int64_t sequence_;

		static std::atomic<int64_t> numCreated_;
	};

	class TimerId{
	public:

		TimerId()
			:timer_(NULL),
			sequence_(0)
		{}

		TimerId(Timer* timer,int64_t seq)
			:timer_(timer),
			sequence_(seq)
		{}

	private:
		Timer* timer_;
		int64_t sequence_;
	};

	class TimerQueue{
	public:
		typedef std::function<void()> TimerCallBack;
		TimerQueue(const TimerQueue&)=delete;
		void operator=(const TimerQueue&)=delete;

		explicit TimerQueue(EventLoop* loop);
		~TimerQueue();

		TimerId addTimer(const TimerCallBack& cb,Timestamp when,double interval);

		void cancel(TimerId timerId);

	private:
		typedef std::pair<Timestamp,Timer*> Entry;
		typedef std::set<Entry> TimerList;
		typedef std::pair<Timer*,int64_t> ActiveTimer;
		typedef std::set<ActiveTimer> ActiveTimerSet;

		void addTimerInLoop(Timer* timer);
		void cancelTimerInLoop(TimerId timerId);

		void handleRead();

		std::vector<Entry> getExpired(Timestamp now);
		void reset(const std::vector<Entry>& expired,Timestamp now);

		bool insert(Timer* timer);

		EventLoop* loop_;
		const int timerfd_;
		Channel timerfdChannel_;

		TimerList timers_;

		ActiveTimerSet activeTimers_;
		bool callingExpiredTimers_;
		ActiveTimerSet cancelingTimers_;
	};

} // namespace server


#endif