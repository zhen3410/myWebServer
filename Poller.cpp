#include"Poller.h"
#include"channel.h"

#include<poll.h>
#include<assert.h>
#include<iostream>

using namespace server;

Poller::Poller(EventLoop* loop)
	: ownerloop_(loop)
{
}

Poller::~Poller(){
}

time_t Poller::poll(int timeoutMs,ChannelList* activeChannels){
	int numEvents=::poll(&*pollfds_.begin(),pollfds_.size(),timeoutMs);

	time_t now=time(0);

	if(numEvents>0){
		std::cout<<numEvents<<" events happended "<<std::endl;
		fillActiveChannels(numEvents,activeChannels);
	}else if(numEvents==0){
		std::cout<<"nothing happended"<<std::endl;
	}else{
		std::cerr<<"Poller::poll()"<<std::endl;
	}
	return now;
}

void Poller::fillActiveChannels(int numEvents,ChannelList* activeChannels)const{
	for(PollFdList::const_iterator pfd=pollfds_.begin();pfd!=pollfds_.end()&&numEvents>0;++pfd){
		if(pfd->revents>0){
			--numEvents;
			ChannelMap::const_iterator ch=channels_.find(pfd->fd);
			assert(ch!=channels_.end());
			Channel* channel=ch->second;
			assert(channel->fd()==pfd->fd);
			channel->setRevents(pfd->revents);
			activeChannels->push_back(channel);
		}
	}
}

void Poller::updateChannel(Channel* channel){
	assertInLoopThread();
	std::cout<<"Poller::updateChannel() fd = "<<channel->fd()<<" events = "<<channel->events()<<std::endl;
	if(channel->index()<0){
		// 是一个新的channel
		assert(channels_.find(channel->fd())==channels_.end());
		struct pollfd pfd;
		pfd.fd=channel->fd();
		pfd.events=static_cast<short>(channel->events());
		pfd.revents=0;
		pollfds_.push_back(pfd);
		int ind=static_cast<int>(pollfds_.size())-1;
		channel->setIndex(ind);
		channels_[pfd.fd]=channel;
	}else{
		// 是一个旧的channel，更新
		assert(channels_.find(channel->fd())!=channels_.end());
		assert(channels_[channel->fd()]==channel);
		int ind=channel->index();
		assert(0<=ind&&ind<static_cast<short>(pollfds_.size()));
		struct pollfd& pfd=pollfds_[ind];
		// 此时文件描述符表示-1的意思是这个channel暂时不关心任何事件
		assert(pfd.fd==channel->fd()||pfd.fd==-1);
		pfd.events=static_cast<short>(channel->events());
		pfd.revents=0;
		if(channel->isNoneEvent()){
			pfd.fd=-1;
		}
	}
}
