#include"TcpConnection.h"
#include"Channel.h"

#include<iostream>
#include<string.h>

TcpConnection::TcpConnection(EventLoop& loop,int fd, const struct sockaddr_in& peerAddr)
    :loop_(loop),
    fd_(fd),
    peerAddr_(peerAddr),
    channelPtr_(new Channel(loop_,fd_))
{
    channelPtr_->setReadCallBack(std::bind(&TcpConnection::readHandle,this));
    channelPtr_->enableReading();
    channelPtr_->setWriteCallBack(std::bind(&TcpConnection::writeHandle,this));
    channelPtr_->enableWriting();
    channelPtr_->setErrorCallBack(std::bind(&TcpConnection::errorHandle,this));

}

TcpConnection::~TcpConnection(){

}

void TcpConnection::readHandle(){
    char buf[4092];
    int n=read(fd_,buf,strlen(buf));
    std::cout<<"recv"<<buf<<std::endl;
}

void TcpConnection::writeHandle(){

}

void TcpConnection::errorHandle(){

}
