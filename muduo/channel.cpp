#include"channel.h"
#include"EventLoop.h"
#include<poll.h>
#include<iostream>
#include<assert.h>

using namespace server;

const int Channel::kNoneEvent=0;
const int Channel::kReadEvent=POLLIN|POLLPRI;
const int Channel::kWriteEvent=POLLOUT;

Channel::Channel(EventLoop* loop,int fd)
	: loop_(loop),
	  fd_(fd),
	  events_(0),
	  revents_(0),
	  index_(-1),
	  eventHandling_(false),
	  addedToLoop_(false)
{

}

Channel::~Channel(){
	assert(!eventHandling_);
	//if(loop_->isInLoopThread()){

	//}
}

void Channel::update(){
	addedToLoop_=true;
	loop_->updateChannel(this);
}

void Channel::remove(){
	assert(isNoneEvent());
	addedToLoop_=false;
	loop_->removeChannel(this);
}

void Channel::handleEvent(Timestamp receiveTime){

	eventHandling_=true;
	if(revents_&POLLNVAL){
		std::cout<<"Channel::handleEvent() POLLNVAL"<<std::endl;
	}
	if((revents_&POLLHUP)&&!(revents_&POLLIN)){
		if(closeCallback_)closeCallback_();
	}
	if(revents_&(POLLERR|POLLNVAL)){
		if(errorCallback_)errorCallback_();
	}
	if(revents_&(POLLIN|POLLPRI|POLLRDHUP)){
		if(readCallback_)readCallback_(receiveTime);
	}
	if(revents_&POLLOUT){
		if(writeCallback_)writeCallback_();
	}
	eventHandling_=false;
}
