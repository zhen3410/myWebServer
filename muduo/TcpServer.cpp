#include"TcpServer.h"
#include"Acceptor.h"
#include"EventLoop.h"
#include"TcpConnection.h"
#include"EventLoopThreadPool.h"

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
	threadPool_(new EventLoopThreadPool(loop)),
	started_(false),
	nextConnId_(1)
{
	std::cout<<"TcpServer::TcpServer() name = ["<<name_<<"]"<<std::endl;
	acceptor_->setNewConnectionCallBack(std::bind(&TcpServer::newConnection,this,_1,_2));
}

TcpServer::~TcpServer(){

}

void TcpServer::setThreadNum(int numThreads){
	assert(numThreads>=0);
	threadPool_->setThreadNum(numThreads);
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
	EventLoop* ioLoop=threadPool_->getNextLoop();

	TcpConnectionPtr conn(new TcpConnection(ioLoop,connName,sockfd,loaclAddr,peerAddr));
	// 如果TcpServer不持有TcpConnection的话，conn会在这一步析构，导致socket文件关闭，连接出错
	connections_[connName]=conn;
	conn->setConnectionCallBack(connectionCallBack_);
	conn->setMessageCallBack(messageCallBack_);
	conn->setWriteCompleteCallBack(writeCompleteCallBack_);
	conn->setCloseCallBack(std::bind(&TcpServer::removeConnection,this,_1));
	//conn->connectionEstablished();
	ioLoop->runInLoop(std::bind(&TcpConnection::connectionEstablished,conn));
}

void TcpServer::removeConnection(const TcpConnectionPtr& conn){
	loop_->assertInLoopThread();
	std::cout<<"TcpServer::removeConnection ["<<name_<<"] - connection ["<<conn->getName()<<"]"<<std::endl;
	loop_->runInLoop(std::bind(&TcpServer::removeConnectionInLoop,this,conn));	
}


void TcpServer::removeConnectionInLoop(const TcpConnectionPtr& conn){
	loop_->assertInLoopThread();
	std::cout<<"TcpServer::removeConnectionInLoop ["<<name_<<"] - connection ["<<conn->getName()<<"]"<<std::endl;
	size_t n=connections_.erase(conn->getName());
	EventLoop* ioLoop=conn->getLoop();
	ioLoop->queueInLoop(std::bind(&TcpConnection::connectionDestroyed,conn));
}
