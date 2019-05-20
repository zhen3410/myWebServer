#include"EPoller.h"
#include"Timestamp.h"
#include"channel.h"

#include<assert.h>
#include<errno.h>
#include<poll.h>
#include<sys/epoll.h>
#include<iostream>

using namespace server;

namespace {
const int kNew=1;
const int kAdded=2;
const int kDeleted=3;
}

EPoller::EPoller(EventLoop* loop)
	:ownerLoop_(loop),
	epollfd_(::epoll_create1(EPOLL_CLOEXEC)),
	events_(16)
{
	if(epollfd_<0){
		std::cerr<<"EPoller::EPoller constructer"<<std::endl;
	}
}

EPoller::~EPoller(){
	::close(epollfd_);
}

Timestamp EPoller::poll(int timeoutMs,ChannelList* activeChannels){
	int numEvents=::epoll_wait(epollfd_,events_.data(),static_cast<int>(events_.size()),timeoutMs);
	Timestamp now(Timestamp::now());
	if(numEvents>0){
		std::cout<<numEvents<<" events happended"<<std::endl;
		fillActiveChannels(numEvents,activeChannels);
		if(static_cast<size_t>(numEvents)==events_.size()){
			events_.resize(events_.size()*2);
		}
	} else if(numEvents==0){
		std::cout<<"nothing happened"<<std::endl;
	}else{
		std::cerr<<"EPoller::poll() error"<<std::endl;
	}
	return now;
}

void EPoller::fillActiveChannels(int numEvents,ChannelList* activeChannels){
	assert(static_cast<size_t>(numEvents)<=events_.size());
	for(int i=0;i<numEvents;i++){
		Channel* channel=static_cast<Channel*>(events_[i].data.ptr);
		channel->setRevents(events_[i].events);
		activeChannels->push_back(channel);
	}
}

void EPoller::updateChannel(Channel* channel){
	assertInLoopThread();

	std::cout<<"fd = "<<channel->fd()<<"events = "<<channel->events()<<std::endl;
	const int index=channel->index();

	if(index==kNew||index==kDeleted){
		int fd=channel->fd();
		if(index==kNew){
			assert(channels_.find(fd)==channels_.end());
			channels_[fd]=channel;
			fds_[channel]=fd;
		}else{
			assert(channels_.find(fd)!=channels_.end());
			assert(channels_[fd]=channel);
		}
		channel->set_index(kAdded);
		update(EPOLL_CTL_ADD,channel);
	}else{
		int fd=channel->fd();
		if(channel->isNoneEvent()){
			update(EPOLL_CTL_DEL,channel);
			channel->set_index(kDeleted);
		}else{
			update(EPOLL_CTL_MOD,channel);
		}
	}
}

void EPoller::removeChannel(Channel* channel){
	assertInLoopThread();
	int fd=channel->fd();
	std::cout<<"fd = "<<fd;
	int index=channel->index();
	size_t n=channels_.erase(fd);
	if(index==kAdded){
		update(EPOLL_CTL_DEL,channel);
	}
	channel->set_index(kNew);
}

void EPoller::update(int operation,Channel* channel){
	struct epoll_event event;
	bzero(&event,sizeof event);
	event.events=channel->events();
	event.data.ptr=channel;
	int fd=channel->fd();
	if(::epoll_ctl(epollfd_,operation,fd,&event)<0){
		std::cerr<<"EPoller::update() epoll_ctl op = "<<operation<<" fd = "<<fd<<std::endl;
	}
}