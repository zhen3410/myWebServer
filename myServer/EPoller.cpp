#include"EPoller.h"
#include"EventLoop.h"
#include"Channel.h"

#include<sys/epoll.h>
#include<assert.h>
#include<iostream>
#include<string.h>
#include<errno.h>

const int EVENTNUM=4096;
const int EPOLLWAIT_TIME=10000;

EPoller::EPoller(EventLoop& loop)
    :loop_(loop),
    epollFd_(epoll_create1(EPOLL_CLOEXEC)),
    events_(EVENTNUM)
{
    assert(epollFd_>=0);
    //std::cout<<"EPoller::constructor epoll fd = "<<epollFd_<<std::endl;
}

EPoller::~EPoller(){

}

void EPoller::add_event(const std::shared_ptr<Channel>& channel){
    int fd=channel->fd();
    assert(fd2channel_.find(fd)==fd2channel_.end());
    struct epoll_event event;
    event.data.fd=fd;
    event.events=channel->events();
    int ret=epoll_ctl(epollFd_,EPOLL_CTL_ADD,fd,&event);
    assert(ret>=0);
    fd2channel_[fd]=channel;
    //std::cout<<"EPoller::add_event() fd = "<<fd<<" add to loop "<<loop_.getThreadId()<<std::endl;
}

void EPoller::del_event(const std::shared_ptr<Channel>& channel){
    int fd=channel->fd();
    assert(fd2channel_.find(fd)!=fd2channel_.end());
    struct epoll_event event;
    event.data.fd=fd;
    event.events=channel->events();
    int ret=epoll_ctl(epollFd_,EPOLL_CTL_DEL,fd,&event);
    assert(ret>=0);
    fd2channel_.erase(fd);
}

void EPoller::mod_event(const std::shared_ptr<Channel>& channel){
    int fd=channel->fd();
    /*
    if(fd2channel_.size()==0||fd2channel_.find(fd)==fd2channel_.end()){
	    //    std::cout<<"EPoller::mod_event add_event"<<std::endl;
	    add_event(channel);
	    return;
    } 
    */
    struct epoll_event event;
    event.data.fd=fd;
    event.events=channel->events();
    int ret=epoll_ctl(epollFd_,EPOLL_CTL_MOD,fd,&event);
    assert(ret>=0);
}


void EPoller::poll(std::vector<std::shared_ptr<Channel>> &activeChannel){
    int numEvents=epoll_wait(epollFd_,&*events_.begin(),static_cast<int>(events_.size()),EPOLLWAIT_TIME);
    assert(numEvents>=0);
    
    activeChannel.clear();
    for(int i=0;i<numEvents;i++){
        // 获取产生事件的文件描述符
        int fd=events_[i].data.fd;
	    //std::cout<<"fd = "<<fd<<" happend"<<std::endl;

        // 获取次文件描述符对应的Channel
        assert(fd2channel_.find(fd)!=fd2channel_.end());
        std::shared_ptr<Channel> curChannel=fd2channel_[fd];

        // 将此Channel的活动事件设置为文件描述符产生的事件
        curChannel->setRevents(events_[i].events);

        // 添加到activeChannel中
        activeChannel.push_back(curChannel);

    }

}
