#ifndef SERVER_EPOLL_H
#define SERVER_EPOLL_H

#include<vector>
#include<memory>
#include<map>

#include"base/MutexLock.h"

struct epoll_event;

class Channel;
class EventLoop;

class EPoller{

public:
    EPoller(const EPoller&)=delete;
    void operator=(const EPoller&)=delete;

    EPoller(EventLoop&);
    ~EPoller();

    void poll(std::vector<std::shared_ptr<Channel>>& avtiveChannel);

    void add_event(std::shared_ptr<Channel>);
    void del_event(std::shared_ptr<Channel>);
    void mod_event(std::shared_ptr<Channel>);

private:
    EventLoop& loop_;
    int epollFd_;
    std::vector<epoll_event> events_;
    std::map<int,std::shared_ptr<Channel>> fd2channel_;

    MutexLock mutex_;
};

#endif
