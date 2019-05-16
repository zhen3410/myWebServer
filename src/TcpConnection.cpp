#include"TcpConnection.h"
#include"Socket.h"
#include"channel.h"
#include"EventLoop.h"

#include<unistd.h>
#include<iostream>

using namespace server;

TcpConnection::TcpConnection(EventLoop* loop,
	std::string name,
	int sockfd,
	const InetAddress& localAddr,
	const InetAddress& peerAddr)
	:loop_(loop),
	name_(name),
	socket_(new Socket(sockfd)),
	channel_(new Channel(loop_,sockfd)),
	localAddr_(localAddr),
	peerAddr_(peerAddr),
	state_(kConnecting)
{
	std::cout<<"TcpConnection::constructor ["<<name_<<"] fd = "<<channel_->fd()<<std::endl;
	channel_->setReadCallback(std::bind(&TcpConnection::handleRead,this));
}

TcpConnection::~TcpConnection(){
	std::cout<<"TcpConnection::deconstructor ["<<name_<<"] fd = "<<channel_->fd()<<std::endl;
}

void TcpConnection::connectionEstablished(){
	loop_->assertInLoopThread();
	std::cout<<"TcpConnection::connectionEstablished() set connection channel fd = ["<<socket_->fd()<<"]"<<std::endl;
	channel_->enableReading();
	setState(kConnected);
	connectionCallBack_(shared_from_this());
}

void TcpConnection::handleRead(){

	char buf[65536];
	ssize_t n=::read(channel_->fd(),buf,sizeof buf);
	messageCallBack_(shared_from_this(),buf,n);

}
