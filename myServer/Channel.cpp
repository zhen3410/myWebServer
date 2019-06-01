#include"Channel.h"

#include<sys/epoll.h>
#include<iostream>

const int Channel::kNoneEvent=0;
const int Channel::kReadEvent=EPOLLIN|EPOLLPRI;
const int Channel::kWriteEvent=EPOLLOUT;
const int Channel::kEdgeTrigger=EPOLLET;

Channel::Channel(EventLoop& loop,int fd)
	:loop_(loop),
	fd_(fd),
	events_(0),
	revents_(0)
{
	std::cout<<"Channel::constructor fd = "<<fd_<<std::endl;
}

Channel::~Channel()
{

}

void Channel::handleEvent(){
	if(revents_&(EPOLLIN|EPOLLPRI|EPOLLRDHUP)){
		if(readCallBack_)readCallBack_();
	}
	if(revents_&(EPOLLERR)){
		if(errorCallBack_)errorCallBack_();
	}
	if(revents_&EPOLLOUT){
		if(writeCallBack_)writeCallBack_();
	}
}
