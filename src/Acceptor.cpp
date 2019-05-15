#include"Acceptor.h"
#include"EventLoop.h"
#include"InetAddress.h"

#include<sys/socket.h>
#include<iostream>
#include<netinet/in.h>

using namespace server;

int createNonblocking(){
	int fd=::socket(AF_INET,SOCK_STREAM|SOCK_NONBLOCK|SOCK_CLOEXEC,IPPROTO_TCP);
	if(fd<0){
		std::cerr<<"createNonblocking()"<<std::endl;
	}
	return fd;
}


Acceptor::Acceptor(EventLoop* loop,const InetAddress& listenAddr)
	:loop_(loop),
	acceptSocket_(createNonblocking()),
	acceptChannel_(loop_,acceptSocket_.fd()),
	listening_(false)
{
	acceptSocket_.setReuseAddr(true);
	acceptSocket_.bindAddr(listenAddr);
	acceptChannel_.setReadCallback(std::bind(&Acceptor::handleRead,this));
}

Acceptor::~Acceptor(){
	acceptChannel_.disableAll();
	acceptChannel_.remove();
	//::close()
}

void Acceptor::listen(){
	loop_->assertInLoopThread();
	listening_=true;
	acceptSocket_.listen();
	acceptChannel_.enableReading();
}

void Acceptor::handleRead(){
	loop_->assertInLoopThread();
	InetAddress peeraddr;
	int connfd=acceptSocket_.accept(&peeraddr);
	if(connfd>=0){
		// 接受了客户机的请求，开始建立新连接，传入连接socket和客户机的地址
		newConnectionCallBack_(connfd,peeraddr);
	}
}
