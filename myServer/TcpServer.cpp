#include"TcpServer.h"
#include"Socket.h"
#include"Channel.h"
#include"TcpConnection.h"

#include<iostream>
#include<string.h>
#include<arpa/inet.h>

TcpServer::TcpServer(EventLoop& loop,int port,const std::string& name)
    :loop_(loop),
    socket_(port),
    name_(name),
    acceptChannel_(new Channel(loop,socket_.fd())),
    timingWheel_(loop_),
    ConnectionId_(0)
{
    acceptChannel_->setReadCallBack(std::bind(&TcpServer::newConnection,this));
    acceptChannel_->enableReading();
}

TcpServer::~TcpServer(){

}

void TcpServer::start(){
    socket_.bindAndListening();
    timingWheel_.start();
    std::cout<<"TcpServer()::start() ["<<name_<<"] started"<<std::endl;
    //timingWheel_.start();
}

void TcpServer::newConnection(){
	struct sockaddr_in peerAddr;
	memset(&peerAddr,0,sizeof peerAddr);
	int connfd=socket_.accept(peerAddr);
    std::string newTcpConnectionName=name_+std::to_string(ConnectionId_++);
	std::cout<<"newConnection() accept a new Connection ["<<newTcpConnectionName<<"] from "
		<<inet_ntoa(peerAddr.sin_addr)<<":"<<ntohs(peerAddr.sin_port)<<std::endl;
	{
    TcpConnectionPtr newConn(std::make_shared<TcpConnection>(loop_,connfd,peerAddr,newTcpConnectionName));
    conn_[newTcpConnectionName]=newConn;
	std::cout<<"TcpServer::newConnection() conn_ num = "<<conn_.size()<<" , newConn use count = "<<newConn.use_count()<<std::endl;
    timingWheel_.addConnection(newConn);
    newConn->setCloseCallBack(std::bind(&TcpServer::ConnectionCloseCallBack,this,std::placeholders::_1));
    newConn->setMessageCallBack(std::bind(&TcpServer::ConnMessageCallBack,this,std::placeholders::_1));
	}
	std::cout<<"TcpServer::newConnection() newConn use count real is "<<conn_[newTcpConnectionName].use_count()<<std::endl;
}

void TcpServer::ConnectionCloseCallBack(const std::string& name){
    auto conn=conn_[name];
    conn_.erase(name);
    std::cout<<"TcpServer::ConnectionCloseCallaBack() conn_ num = "<<conn_.size()<<" , eraseConn use count = "<<conn.use_count()<<std::endl;
    // 为了channel对象声明管理的需要，将channel解注册移至loop中执行
    loop_.queueInLoop(std::bind(&TcpConnection::connectionDestroy,conn));
}

void TcpServer::ConnMessageCallBack(const TcpConnectionPtr& conn){
    timingWheel_.touchTimer(conn->name());
    messageCallBack_(conn);
}
