#ifndef SERVER_TIMINGWHEEL_H
#define SERVER_TIMINGWHEEL_H

#include"TcpConnection.h"

#include<unordered_set>
#include<memory>
#include<deque>

class TimingWheel
{
public:
    TimingWheel(/* args */);
    ~TimingWheel();
private:
    typedef std::weak_ptr<TcpConnection> weakPtrTcpConnection;
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
};


#endif