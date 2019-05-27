#ifndef SERVER_EPOLL_H
#define SERVER_EPOLL_H

#include<vector>
#include<memory>
#include<map>

struct epoll_event;

class Channel;
class EventLoop;

class EPoller{

public:
    EPoller(const EPoller&)=delete;
    void operator=(const EPoller&)=delete;

    EPoller();
    ~EPoller();

    void poll(std::vector<std::shared_ptr<Channel>>& avtiveChannel);

    void add_event(int epollfd,int fd,int state);
    void del_event(int epollfd,int fd,int state);
    void mod_event(int epollfd,int fd,int state);

private:
    EventLoop& loop_;
    int epollFd_;
    std::vector<epoll_event> events_;
    std::map<int,std::shared_ptr<Channel>> fd2channel_;
};

#endif