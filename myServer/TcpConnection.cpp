#include"TcpConnection.h"
#include"Channel.h"

#include<iostream>
#include<string.h>
#include<arpa/inet.h>
#include<unistd.h>

//const int TcpConnection::BUFFERSIZE=4096;

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
    channelPtr_->enableWriting();
    channelPtr_->setErrorCallBack(std::bind(&TcpConnection::errorHandle,this));
    channelPtr_->setET();
}

TcpConnection::~TcpConnection(){
	::close(fd_);
}

void TcpConnection::readHandle(){
    char buf[4092];
    int n=readn(fd_,inputBuffer_);
    if(n>0){
	    std::cout<<"recv "<<strlen(buf)<<" bytes"<<std::endl;
        // 执行消息回调函数
        messageCallBack_(shared_from_this());
    }else if(n==0){
        closeHandle();
    }else{
        errorHandle();
    }
}

void TcpConnection::writeHandle(){
    int n=writen(fd_,outputBuffer_);
    if(n>0){
        if(outputBuffer_.size()>0){
            channelPtr_->enableWriting();
        }
    }else if(n<0){
        errorHandle();
    }else{
        closeHandle();
    }
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
    outputBuffer_+=msg;
    writeHandle();
}

ssize_t readn(int fd,std::string& buffer){
    size_t left=65536;
    size_t nread;
    ssize_t tot=0;
    for(;;){
        char ptr[65536];
        nread=read(fd,ptr,left);
        if(nread<0){
            if(errno==EINTR)
                continue;
            else if(errno==EAGIN)
                return tot;
            else return -1;
        }else if(nread==0)
            return tot;
        tot+=nread;
        buffer+=std::string(ptr,nread);
    }
    return tot;
}

ssize_t writen(int fd,const std::string& buffer){
    size_t left=buffer.size();
    ssize_t nWriten=0;
    ssize_t tot=0;
    const char* ptr=buffer.c_str();
    while(left>0){
        nWriten=write(fd,ptr,left);
        if(nWriten<=0){
            if(nWriten<0&&errno==EINTR)
                continue;
            else if(nWriten<0&&errno==EAGIN)
                break;
            else return -1;
        }
        ptr+=nWriten;
        left-=nWriten;
        tot+=nWriten;
    }
    if(nWriten==buffer.size())
        buffer.clear();
    else buffer=buffer.substr(nWriten);
    return tot;
}

