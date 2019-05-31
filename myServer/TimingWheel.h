#ifndef SERVER_TIMINGWHEEL_H
#define SERVER_TIMINGWHEEL_H

#include"TcpConnection.h"
#include"base/MutexLock.h"
#include"base/Condition.h"
#include"base/Thread.h"
#include"base/CountDownLatch.h"

#include<unordered_set>
#include<memory>
#include<deque>
#include<functional>

class EventLoop;

class TimingWheel
{
public:
    typedef std::weak_ptr<TcpConnection> weakPtrTcpConnection;
    TimingWheel(/* args */);
    ~TimingWheel();

    void start();

    void addConnection(weakPtrTcpConnection);

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
                conn->shutdown();
        }
        /* data */

        weakPtrTcpConnection weakConn_;
    };
      
    typedef std::shared_ptr<Entry> EntryPtr;
    typedef std::weak_ptr<EntryPtr> WeakEntryPtr;
    typedef std::unordered_set<WeakEntryPtr> Bucket;
    typedef std::deque<Bucket> TimingWheelQueue;

    TimingWheelQueue timingWheel_;
    Thread thread_;
    CountDownLatch latch_;
    MutexLock mutex_;
    Condition cond_;
};


#endif