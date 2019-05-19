#include"TcpConnection.h"
#include"Socket.h"
#include"channel.h"
#include"EventLoop.h"

#include<unistd.h>
#include<iostream>
#include<assert.h>
#include<errno.h>

using namespace server;
using std::placeholders::_1;

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
	channel_->setReadCallback(std::bind(&TcpConnection::handleRead,this,_1));
	channel_->setWriteCallback(std::bind(&TcpConnection::handleWrite,this));
	channel_->setCloseCallback(std::bind(&TcpConnection::handleClose,this));
}

TcpConnection::~TcpConnection(){
	std::cout<<"TcpConnection::deconstructor ["<<name_<<"] fd = "<<channel_->fd()<<std::endl;
}

void TcpConnection::connectionEstablished(){
	loop_->assertInLoopThread();
	std::cout<<"TcpConnection::connectionEstablished() set connection channel fd = ["<<socket_->fd()<<"]"<<std::endl;
	assert(state_==kConnecting);
	channel_->enableReading();
	setState(kConnected);
	connectionCallBack_(shared_from_this());
}

void TcpConnection::connectionDestroyed(){
	loop_->assertInLoopThread();
	std::cout<<"TcpConnection::connectionDistroyed() connection ["<<name_<<"] closed"<<std::endl;
	assert(state_==kConnected||state_==kDisconnecting);
	setState(kDisconnected);
	channel_->disableAll();
	connectionCallBack_(shared_from_this());

	loop_->removeChannel(channel_.get());
}

void TcpConnection::handleRead(Timestamp receiveTime){

/*
	char buf[65536];
	ssize_t n=::read(channel_->fd(),buf,sizeof buf);
	if(n>0){
		messageCallBack_(shared_from_this(),buf,n);
	}else if(n==0){
		handleClose();
	}else{
		std::cout<<"TcpConnection::handleRead() read error"<<std::endl;
	}
*/
	int savedErrno=0;
	ssize_t n=inputBuffer_.readFd(channel_->fd(),&savedErrno);
	if(n>0){
		messageCallBack_(shared_from_this(),&inputBuffer_,receiveTime);
	}else if(n==0){
		handleClose();
	}else{
		errno=savedErrno;
		std::cout<<"TcpConnection::handleRead errno = "<<strerror(errno)<<std::endl;
	}
}

void TcpConnection::handleClose(){
	loop_->assertInLoopThread();
	assert(state_==kConnected||state_==kDisconnecting);
	channel_->disableAll();
	closeCallBack_(shared_from_this());
}

void TcpConnection::shutdown(){
	if(state_==kConnected){
		setState(kDisconnecting);
		loop_->runInLoop(std::bind(&TcpConnection::shutdownInLoop,this));
	}
}

void TcpConnection::setTcpNoDelay(bool on){
	socket_->setTcpNoDelay(on);
}

void TcpConnection::shutdownInLoop(){
	loop_->assertInLoopThread();
	if(!channel_->isWriting()){
		socket_->shutdownWrite();
	}
}

void TcpConnection::send(const std::string& message){
	if(state_==kConnected){
		loop_->runInLoop(std::bind(&TcpConnection::sendInLoop,this,message));
	}
}

void TcpConnection::sendInLoop(const std::string& message){
	loop_->assertInLoopThread();
	ssize_t nwrote=0;
	if(!channel_->isWriting()&&outputBuffer_.readableBytes()==0){
		nwrote=::write(channel_->fd(),message.data(),message.size());
		if(nwrote>=0){
			if(static_cast<size_t>(nwrote)<message.size()){
				std::cout<<"data write incompleted"<<std::endl;
			}else if(writeCompleteCallBack_){
				loop_->runInLoop(std::bind(writeCompleteCallBack_,shared_from_this()));
			}
		}else{
			nwrote=0;
			std::cerr<<"TcpConnection::sendInLoop write error "<<std::endl;
		}
	}
	assert(nwrote>=0);
	if(static_cast<size_t>(nwrote)<message.size()){
		outputBuffer_.append(message.data()+nwrote,message.size()-nwrote);
		if(!channel_->isWriting()){
			channel_->enableWriting();
		}
	}
}

void TcpConnection::handleWrite(){
	loop_->assertInLoopThread();
	if(channel_->isWriting()){
		ssize_t n=::write(channel_->fd(),outputBuffer_.peek(),outputBuffer_.readableBytes());
		if(n>0){
			outputBuffer_.retrieve(n);
			if(outputBuffer_.readableBytes()==0){
				channel_->disableWriting();
				if(writeCompleteCallBack_){
					loop_->runInLoop(writeCompleteCallBack_,shared_from_this());
				}
				if(state_==kDisconnecting){
					shutdownInLoop();
				}
			}else{
				std::cout<<"TcpConnection::handleWrite is writing more data"<<std::endl;
			}
		}else{
			std::cerr<<"TcpConnection::handleWrite"<<std::endl;
		}
	}else{
		std::cout<<"Connection is down, no more writing."<<std::endl;
	}
}
