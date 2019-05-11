#ifndef SERVER_TIMER_H
#define SERVER_TIMER_H

#include<vector>
#include<set>
#include<map>
#include<functional>

namespace server{

	class EventLoop;
	class Channel;
	class Timestamp;

	class TimerQueue{
	public:
		typedef std::function<void()> TimerCallBack;
		TimerQueue(const TimerQueue&)=delete;
		void operator=(const TimerQueue&)=delete;

		int addTimer(const TimerCallBack& cb,Timestamp when);

		void cancel(int timerId);

	private:
		typedef std::pair<Timestamp,Timer*> Entry;
		typedef std::set<Entry> TimerList;

		void handleRead();

		std::vector<Entry> getExpired(Timestamp now);
		void reset(const std::vector<Entry>& expired,Timestamp now);

		bool insert(Timer* timer);

		EventLoop* loop_;
		const int timerfd_;
		Channel timerfdChannel_;

		TimerList timers_;
	};

	class Timer{
	public:
		typedef std::function<void()> TimerCallBack;
		Timer(TimerCallBack cd,Timestamp when)
			:callBack_(std::move(cb)),
			expired_(when){}

		void run() const{
			callBack_();
		}

	private:
		const TimerCallBack callBack_;
		Timestamp expired_;
	}

} // namespace server


#endif