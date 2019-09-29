#ifndef SERVER_TIMINGWHEEL_H
#define SERVER_TIMINGWHEEL_H

#include"TcpConnection.h"
#include"base/MutexLock.h"

#include<unordered_set>
#include<memory>
#include<deque>
#include<functional>
#include<map>
#include<string>
#include<iostream>

class EventLoop;

class TimingWheel
{
public:
    typedef std::weak_ptr<TcpConnection> weakPtrTcpConnection;
    typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
    TimingWheel(EventLoop& loop);
    ~TimingWheel();

    void start();

    void addConnection(const TcpConnectionPtr&);
    void onTimer();
    void touchTimer(const std::string&);

private:
    void threadFunc();
    /* data */
    struct Entry
    {
        explicit Entry(const weakPtrTcpConnection& weakConn)
            :weakConn_(weakConn)
        {
		//std::cout<<"Entry constructor weakConn_.use_count = "<<weakConn_.use_count()<<std::endl;
	    }

        ~Entry(){
		//std::cout<<"Entry deconstructor weakConn_.use_count = "<<weakConn_.use_count()<<std::endl;
            auto conn=weakConn_.lock();
            if(conn){
               // conn2entry_.erase(conn->name());
                conn->closeTimeout();
            }
        }
        /* data */

        weakPtrTcpConnection weakConn_;
    };
    typedef std::shared_ptr<Entry> EntryPtr;
    typedef std::weak_ptr<Entry> WeakEntryPtr;
    typedef std::unordered_set<EntryPtr> Bucket;
    typedef std::deque<Bucket> TimingWheelQueue;


    EventLoop& loop_;
    int fd_;
    std::shared_ptr<Channel> clearConnChannel_;

    TimingWheelQueue timingWheel_;
    MutexLock mutex_;
    // 这里不能使用shared_ptr<Entry> 作为键值，否则会导致引用计数增加，无法正确删除连接
    std::map<std::string,WeakEntryPtr> conn2entry_={};

    static const int kDestroySeconds;
};


#endif
