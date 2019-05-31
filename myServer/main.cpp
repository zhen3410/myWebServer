#include"EventLoop.h"
#include"base/Thread.h"
#include"Channel.h"

#include<iostream>
#include<unistd.h>
#include<string.h>
#include<sys/timerfd.h>
#include<memory>

EventLoop* g_loop;

void ThreadFunc(){
	//g_loop->loop();
	std::cout<<"threadFunc() pid = "<<getpid()<<" , tid = "<<CurrentThread::tid()<<std::endl;
	EventLoop loop;
	loop.loop();
}

void timeout(){
	std::cout<<"Timeout!"<<std::endl;
	g_loop->quit();
}

int main(){
	std::cout<<"main() pid = "<<getpid()<<" , tid = "<<CurrentThread::tid()<<std::endl;
	EventLoop loop;
	g_loop=&loop;

	int timerfd=timerfd_create(CLOCK_MONOTONIC,TFD_NONBLOCK|TFD_CLOEXEC);
	std::shared_ptr<Channel> pCh(new Channel(loop,timerfd));
	pCh->setReadCallBack(timeout);
	pCh->enableReading();

	struct itimerspec howlong;
	bzero(&howlong,sizeof howlong);
	howlong.it_value.tv_sec=5;
	timerfd_settime(timerfd,0,&howlong,NULL);

	loop.loop();
	return 0;
}
