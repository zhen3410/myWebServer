#include"TcpConnection.h"
#include"Socket.h"
#include"channel.h"
#include"EventLoop.h"

#include<unistd.h>
#include<iostream>
#include<assert.h>

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

void TcpConnection::connectionDestroyed(){
	loop_->assertInLoopThread();
	std::cout<<"TcpConnection::connectionDistroyed() connection ["<<name_<<"] closed"<<std::endl;
	assert(state_==kConnected);
	//setState(kDisconnected);
	channel_->disableAll();
	connectionCallBack_(shared_from_this());

	loop_->removeChannel(channel_.get());
}

void TcpConnection::handleRead(){

	char buf[65536];
	ssize_t n=::read(channel_->fd(),buf,sizeof buf);
	if(n>0){
		messageCallBack_(shared_from_this(),buf,n);
	}else if(n==0){
		handleClose();
	}else{
		std::cout<<"TcpConnection::handleRead() read error"<<std::endl;
	}

}

void TcpConnection::handleClose(){
	loop_->assertInLoopThread();
	assert(state_==kConnected);
	channel_->disableAll();
	closeCallBack_(shared_from_this());
}
