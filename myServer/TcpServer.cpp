#include"TcpServer.h"
#include"Socket.h"
#include"Channel.h"
#include"TcpConnection.h"
#include"EventLoopThreadPool.h"

#include<iostream>
#include<string.h>
#include<arpa/inet.h>

TcpServer::TcpServer(EventLoop& loop,int port,const std::string& name)
    :loop_(loop),
    //将Acceptor简化为socket
    socket_(port),
    name_(name),
    acceptChannel_(new Channel(loop,socket_.fd())),
    timingWheel_(loop_),
    ConnectionId_(0),
    threadPool_(new EventLoopThreadPool(loop_,name))
{
    acceptChannel_->setReadCallBack(std::bind(&TcpServer::newConnection,this));
}

TcpServer::~TcpServer(){

}

void TcpServer::start(){
    socket_.bindAndListening();
    socket_.setReuseAddr(true);
    socket_.setReusePort(true);
    timingWheel_.start();
    threadPool_->start();
    acceptChannel_->enableReading();
    acceptChannel_->setET();
    acceptChannel_->addChannel();
    //std::cout<<"TcpServer()::start() ["<<name_<<"] started"<<std::endl;
}

void TcpServer::newConnection(){
    loop_.assertInLoopThread();
	struct sockaddr_in peerAddr;
	memset(&peerAddr,0,sizeof peerAddr);
    int connfd;
    // 采用边缘触发，因此当socket可读时不断accept，直到不能accept
	while((connfd=socket_.accept(peerAddr))>=0){

        // 限制服务器最大并发连接数
        if(connfd>maxConn_){
            close(connfd);
            continue;
        }

        std::string newTcpConnectionName=name_+std::to_string(ConnectionId_++);
	    //std::cout<<"newConnection() accept a new Connection ["<<newTcpConnectionName<<"] from "
    	//	<<inet_ntoa(peerAddr.sin_addr)<<":"<<ntohs(peerAddr.sin_port)
        //    <<" , conn fd = "<<connfd<<std::endl;
        EventLoop* ioLoop=threadPool_->getNextLoop();
        TcpConnectionPtr newConn(std::make_shared<TcpConnection>(*ioLoop,connfd,peerAddr,newTcpConnectionName));
        newConn->setTCPNoDelay(true);
        conn_[newTcpConnectionName]=newConn;
        timingWheel_.addConnection(newConn);
        newConn->setCloseCallBack(std::bind(&TcpServer::ConnectionCloseCallBack,this,std::placeholders::_1));
        newConn->setMessageCallBack(std::bind(&TcpServer::ConnMessageCallBack,this,std::placeholders::_1));
        newConn->setConnectionCallBack(connectionCallBack_);
        ioLoop->runInLoop(std::bind(&TcpConnection::connectionEstablished,newConn));
    }
}

void TcpServer::ConnectionCloseCallBack(const std::string& name){
    loop_.runInLoop(std::bind(&TcpServer::ConnectionCloseCallBackInLoop,this,name));
}

void TcpServer::ConnectionCloseCallBackInLoop(const std::string& name){
    loop_.assertInLoopThread();
    auto conn=conn_[name];
    conn_.erase(name);
    //std::cout<<"TcpServer::ConnectionCloseCallaBack() conn_ num = "<<conn_.size()<<" , eraseConn use count = "<<conn.use_count()<<std::endl;
    // 为了channel对象声明管理的需要，将channel解注册移至该TcpConnection所属的loop中执行
    // bugfix 注意这里必须是TCPConnection所属的loop中
    EventLoop& ioLoop=conn->getLoop();
    ioLoop.queueInLoop(std::bind(&TcpConnection::connectionDestroy,conn));
}


void TcpServer::ConnMessageCallBack(const TcpConnectionPtr& conn){
    timingWheel_.touchTimer(conn->name());
    messageCallBack_(conn);
}
