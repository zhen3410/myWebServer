#include"TcpServer.h"
#include"Acceptor.h"
#include"EventLoop.h"
#include"TcpConnection.h"

#include<netinet/in.h>
#include<functional>
#include<memory>
#include<iostream>

using namespace server;
using std::placeholders::_1;
using std::placeholders::_2;

TcpServer::TcpServer(EventLoop* loop,const InetAddress& listenAddr,std::string name)
	:loop_(loop),
	name_(name),
	acceptor_(new Acceptor(loop_,listenAddr)),
	started_(false),
	nextConnId_(1)
{
	std::cout<<"TcpServer::TcpServer() name = ["<<name_<<"]"<<std::endl;
	acceptor_->setNewConnectionCallBack(std::bind(&TcpServer::newConnection,this,_1,_2));
}

TcpServer::~TcpServer(){

}

void TcpServer::start(){

	std::cout<<"TcpServer::start() ["<<name_<<"] started"<<std::endl;
	if(!started_){
		started_=true;
	}

	if(!acceptor_->listenning()){
		loop_->runInLoop(std::bind(&Acceptor::listen,acceptor_.get()));
	}
}

void TcpServer::newConnection(int sockfd,const InetAddress& peerAddr){
	loop_->assertInLoopThread();
	std::string connName=name_+std::to_string(nextConnId_++);
	std::cout<<"TcpServer::newConnection ["<<name_<<"] - new connection ["
	<<connName<<"] from "<<peerAddr.getInfo()<<std::endl;

	struct sockaddr_in localAddr_in;
	socklen_t addrLen=static_cast<socklen_t>(sizeof localAddr_in);
	getsockname(sockfd,(struct sockaddr*)(&localAddr_in),&addrLen);
	InetAddress loaclAddr(localAddr_in);

	TcpConnectionPtr conn(new TcpConnection(loop_,connName,sockfd,loaclAddr,peerAddr));
	connections_[connName]=conn;
	conn->setConnectionCallBack(connectionCallBack_);
	conn->setMessageCallBack(messageCallBack_);
	conn->connectionEstablished();

}
