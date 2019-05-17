#include"TcpServer.h"
#include"EventLoop.h"
#include"InetAddress.h"
#include"TcpConnection.h"

#include<iostream>
#include<unistd.h>
#include<sys/timerfd.h>

EventLoop* g_loop;

void timeout(server::Timestamp reveiveTime){
	std::cout<<reveiveTime.get()<<" timeout!"<<std::endl;
	g_loop->quit();
}

int main(){

	std::cout<<"main() : pid = "<<getpid()<<std::endl;
	
	//server::InetAddress listenAddr(9981);
	server::EventLoop loop;
	g_loop=&loop;

	int timerfd=::timerfd_create(CLOCK_MONOTONIC,TFD_NONBLOCK|TFD_CLOEXEC);
	server::Channel channel(&loop,timerfd);
	channel.setReadCallBack(timeout);
	channel.enableReading();

	struct itimerspec howlong;
	bzero(&howlong,sizeof howlong);
	howlong.it_valur.tv_sec=5;
	::timerfd_settime(timerfd,0,&howlong,NULL);

	loop.loop();

	::close(timerfd);
	return 0;
}
