#include"TcpConnection.h"
#include"Channel.h"

#include<iostream>
#include<string.h>
#include<arpa/inet.h>
#include<unistd.h>

//const int TcpConnection::BUFFERSIZE=4096;

ssize_t readn(int fd,std::string& buffer){
	//必须使用ssize_t,為有符號整形
    ssize_t nread;
    ssize_t tot=0;
    for(;;){
        char ptr[4096];
        nread=read(fd,ptr,4096);
	//std::cout<<"read "<<nread<<" bytes"<<std::endl;
        if(nread<0){
            if(errno==EINTR)
                continue;
            else if(errno==EAGAIN){
                return tot;
	    }
            else return -1;
        }else if(nread==0){
            return tot;
	}
        tot+=nread;
	//std::cout<<"tot read "<<tot<<" bytes "<<std::endl;
        buffer+=std::string(ptr,ptr+nread);
    }
    return tot;
}

ssize_t writen(int fd,std::string& buffer){
    size_t left=buffer.size();
    ssize_t nWriten=0;
    ssize_t tot=0;
    const char* ptr=buffer.c_str();
    while(left>0){
        nWriten=write(fd,ptr,left);
        if(nWriten<=0){
            if(nWriten<0&&errno==EINTR)
                continue;
            else if(nWriten<0&&errno==EAGAIN){
                break;
	    }
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

TcpConnection::TcpConnection(EventLoop& loop,int fd, const struct sockaddr_in& peerAddr,const std::string& name)
    :loop_(loop),
    fd_(fd),
    peerAddr_(peerAddr),
    channelPtr_(new Channel(loop_,fd_)),
    name_(name),
	inputBuffer_(),
	outputBuffer_(),
    halfClose_(false),
    disConnected_(false)
{
    channelPtr_->setReadCallBack(std::bind(&TcpConnection::readHandle,this));
    channelPtr_->enableReading();
    channelPtr_->setWriteCallBack(std::bind(&TcpConnection::writeHandle,this));
    //channelPtr_->enableWriting(); 不能再此設置可讀，否則邊緣觸發，會在一瞬間立即退出
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
	    std::cout<<"recv message : "<<inputBuffer_<<std::endl;
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
        }else if(outputBuffer_.size()==0&&halfClose_)  //数据已发送完毕且设置了半关闭位，则关闭连接
            closeHandle();
    }else if(n==0){
        closeHandle();
    }else{
        errorHandle();
    }
}

void TcpConnection::errorHandle(){
    if(disConnected)return;

    disConnected=true;
}

void TcpConnection::closeHandle(){
    std::cout<<"TcpConnection() name =["<<name_<<"] ip:port = "<<inet_ntoa(peerAddr_.sin_addr)<<":"<<ntohs(peerAddr_.sin_port)<<" closed"<<std::endl;
    if(disConnected)return;
    // 有数据没有发送完毕或者刚刚收到数据
    if(outputBuffer_.size()>0||inputBuffer_.size()>0){
        halfClose_=true;
        if(inputBuffer_.size()>0){
            messageCallBack_(shared_from_this());
        }
        return;
    }
    channelPtr_->disableAll();
    closeCallBack_(name_);
    disConnected=true;
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
    //std::cout<<"TcpConnection()::sendInLoop() send = "<<outputBuffer_<<std::endl;
    writeHandle();
}

void TcpConnection::closeTimeout(){
    closeHandle();
    std::cout<<"TcpConnection::closeTimeout() connection ["<<name_<<"] time out"<<std::endl;
}
