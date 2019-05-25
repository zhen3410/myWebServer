#ifndef SERVER_EVENTLOOP_H
#define SERVER_EVENTLOOP_H

#include"Timer.h"
#include"./base/Timestamp.h"
#include"./base/Mutex.h"

#include<pthread.h>
#include<vector>
#include<memory>
#include<sys/syscall.h>
#include<unistd.h>

namespace server{

class EPoller;
class Channel;

class EventLoop {

public:

    typedef std::function<void()> TimerCallBack;
    typedef std::function<void()> functor;
    
    EventLoop(const EventLoop&)=delete;
    void operator=(const EventLoop&)=delete;

    EventLoop();
    ~EventLoop();

    void runInLoop(const functor& cb);
    void queueInLoop(const functor& cb);

    void loop();
    void quit();

    Timestamp pollReturnTime()const{
        return pollReturnTime_;
    }

    void updateChannel(Channel* channel);
    void removeChannel(Channel* channel);

    TimerId runAt(const Timestamp& time,const TimerCallBack& cb){
        return timerQueue_->addTimer(cb,time,0.0);
    }

    TimerId runAfter(double delay,const TimerCallBack& cb){
        Timestamp when(addTime(Timestamp::now(),delay));
        return runAt(when,cb);
    }

    TimerId runEvery(double interval,const TimerCallBack& cb){
        Timestamp time(addTime(Timestamp::now(),interval));
        return timerQueue_->addTimer(cb,time,interval);
    }

    void assertInLoopThread(){
        if(!isInLoopThread()){
            abortNotInLoopThread();
        }
    }
    bool isInLoopThread()const{
        return threadId_==syscall(SYS_gettid);
    }

    EventLoop* getEventLoopOfCurrentThread();

private:
    
    void abortNotInLoopThread();
    void handleRead();
    void doPendingFunctors();
    void wakeup();

    typedef std::vector<Channel*> ChannelList;

    bool looping_;
    bool quit_;
    bool callingPendingFunctors_;
    const pid_t threadId_;

    Timestamp pollReturnTime_;

    std::unique_ptr<EPoller> poller_;
    std::unique_ptr<TimerQueue> timerQueue_;
    int wakeupFd_;
    std::unique_ptr<Channel> wakeupChannel_;
    ChannelList activeChannels_;
    Mutex mutex_;
    std::vector<functor> pendingFunctors_;
};

}

#endif
