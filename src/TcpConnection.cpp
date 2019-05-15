#include"TcpConnection.h"

using namespace server;

TcpConnection::TcpConnection(EventLoop* loop,
	std::string name,
	int sockfd,
	const InetAddress& localAddr,
	const InetAddress& peerAddr)
	:loop_(loop),
	name_(name),
	sockfd_(new Socket(sockfd)),
	channel_(new Channel(loop_,sockfd_)),
	localAddr_(localAddr),
	peerAddr_(peerAddr)
{
	channel_->setReadCallBack(std::bind(&TcpConnection::handleRead,this));
}

TcpConnection::~TcpConnection(){

}

void TcpConnection::connectionEstablished(){
	loop_->assertLoopInThread();
	channel_->enableReading();

	connectionCallBack_(shared_from_this());
}

void TcpConnection::handleRead(){

	char buf[65536];
	ssize_t n=::read(channel_->fd(),buf,sizeof buf);
	messageCallBack_(shared_from_this(),buf,n);

}