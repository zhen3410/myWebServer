#ifndef SERVER_TIMINGWHEEL_H
#define SERVER_TIMINGWHEEL_H

#include"TcpConnection.h"
#include"base/MutexLock.h"

#include<unordered_set>
#include<memory>
#include<deque>
#include<functional>

class EventLoop;

class TimingWheel
{
public:
    typedef std::weak_ptr<TcpConnection> weakPtrTcpConnection;
    typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
    TimingWheel(EventLoop& loop);
    ~TimingWheel();

    void start();

    void addConnection(TcpConnectionPtr);
    void onTimer();
    void touchTimer(TcpConnectionPtr)

private:
    void threadFunc();
    /* data */
    struct Entry
    {
        explicit Entry(const weakPtrTcpConnection& weakConn)
            :weakConn_(weakConn)
        {}

        ~Entry(){
            auto conn=weakConn_.lock();
            if(conn)
                conn->closeTimeout();
        }
        /* data */

        weakPtrTcpConnection weakConn_;
    };
      
    typedef std::shared_ptr<Entry> EntryPtr;
    typedef std::weak_ptr<EntryPtr> WeakEntryPtr;
    typedef std::unordered_set<EntryPtr> Bucket;
    typedef std::deque<Bucket> TimingWheelQueue;

    EventLoop& loop_;
    int fd_;
    std::shared_ptr<Channel> clearConnChannel_;

    TimingWheelQueue timingWheel_;
    CountDownLatch latch_;
    MutexLock mutex_;
    Condition cond_;

    static const int kDestroySeconds;
};


#endif