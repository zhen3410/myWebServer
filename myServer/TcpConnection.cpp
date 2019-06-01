#include"TcpConnection.h"
#include"Channel.h"

#include<iostream>
#include<string.h>
#include<arpa/inet.h>
#include<unistd.h>

TcpConnection::TcpConnection(EventLoop& loop,int fd, const struct sockaddr_in& peerAddr,const std::string& name)
    :loop_(loop),
    fd_(fd),
    peerAddr_(peerAddr),
    channelPtr_(new Channel(loop_,fd_)),
    name_(name)
{
    channelPtr_->setReadCallBack(std::bind(&TcpConnection::readHandle,this));
    channelPtr_->enableReading();
    channelPtr_->setWriteCallBack(std::bind(&TcpConnection::writeHandle,this));
    //channelPtr_->enableWriting();
    channelPtr_->setErrorCallBack(std::bind(&TcpConnection::errorHandle,this));

}

TcpConnection::~TcpConnection(){
	::close(fd_);
}

void TcpConnection::readHandle(){
    char buf[4092];
    int n=read(fd_,buf,4092);
    if(n>0){
	    std::cout<<"recv "<<strlen(buf)<<" bytes"<<std::endl;
        // 执行消息回调函数
        messageCallBack_();
    }else if(n==0){
        closeHandle();
    }else{
        errorHandle();
    }
}

void TcpConnection::writeHandle(){

}

void TcpConnection::errorHandle(){

}

void TcpConnection::closeHandle(){
    std::cout<<"TcpConnection() name =["<<name_<<"] ip:port = "<<inet_ntoa(peerAddr_.sin_addr)<<":"<<ntohs(peerAddr_.sin_port)<<" closed"<<std::endl;
    channelPtr_->disableAll();
    closeCallBack_(name_);
}

void TcpConnection::connectionDestroy(){
    channelPtr_->disableAll();
    loop_.removeChannel(channelPtr_);
}

void TcpConnection::send(const std::string& msg){
    if(loop_.isInLoopThread()){
        sendInLoop(msg);
    }else{
        loop_.runInLoop(std::bind(&TcpConnection::sendInLoop,this,msg));
    }
}

void TcpConnection::sendInLoop(const std::string& msg){
    int n=write(fd_,msg.c_str(),msg.size());
    assert(n>=0);
}