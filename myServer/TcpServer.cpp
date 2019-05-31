#include"TcpServer.h"
#include"Socket.h"
#include"Channel.h"
#include"TcpConnection.h"

#include<iostream>
#include<string.h>

TcpServer::TcpServer(EventLoop& loop,int port,const std::string& name)
    :loop_(loop),
    socket_(port),
    name_(name),
    acceptChannel_(new Channel(loop,socket_.fd())),
    ConnectionId_(0)
{
    acceptChannel_->setReadCallBack(std::bind(&TcpServer::newConnection,this));
    acceptChannel_->enableReading();
}

TcpServer::~TcpServer(){

}

void TcpServer::start(){
    socket_.bindAndListening();
    std::cout<<"TcpServer()::start() ["<<name_<<"] started"<<std::endl;
}

void TcpServer::newConnection(){
	struct sockaddr_in peerAddr;
	memset(&peerAddr,0,sizeof peerAddr);
	int connfd=socket_.accept(peerAddr);
	std::cout<<"newConnection() accept a new Connection from "
		<<ntohl(peerAddr.sin_addr.s_addr)<<":"<<ntohs(peerAddr.sin_port)<<std::endl;
    TcpConnectionPtr newConn(new TcpConnection(loop_,connfd,peerAddr));
    std::string newTcpConnectionName=name_+std::to_string(ConnectionId_);
    conn_[newTcpConnectionName]=newConn;

}
