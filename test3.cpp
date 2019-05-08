#include"channel.h"
#include"EventLoop.h"

#include<iostream>
#include<sys/timerfd.h>
#include<string.h>

server::EventLoop* g_loop;

void timeout(){
	std::cout<<"timeout"<<std::endl;
	g_loop->quit();
}

int main(){
	server::EventLoop loop;
	g_loop=&loop;

	int timerfd=::timerfd_create(CLOCK_MONOTONIC,TFD_NONBLOCK|TFD_CLOEXEC);
	server::Channel channel(&loop,timerfd);
	channel.setReadCallback(timeout);
	channel.enableReading();

	struct itimerspec howlong;
	bzero(&howlong,sizeof(howlong));
	howlong.it_value.tv_sec=5;
	::timerfd_settime(timerfd,0,&howlong,NULL);

	loop.loop();

	::close(timerfd);
}
