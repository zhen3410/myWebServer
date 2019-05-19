#include"Socket.h"
#include"InetAddress.h"

#include<sys/socket.h>
#include<iostream>
#include<unistd.h>

using namespace server;

const int Socket::maxListenListNum=2048;

Socket::~Socket(){
	::close(socketFd_);
}


void Socket::setReuseAddr(bool flag){
	int optval=flag?1:0;
	::setsockopt(socketFd_,SOL_SOCKET,SO_REUSEADDR,&optval,
		static_cast<socklen_t>(sizeof optval));
}

void Socket::bindAddr(const InetAddress& addr){
	struct sockaddr_in addr_;
	addr.get(&addr_);
	int ret=::bind(socketFd_,(struct sockaddr*)(&addr_),sizeof(struct sockaddr_in));
	if(ret==-1){
		std::cerr<<"Socket::bindAddr() error"<<std::endl;
	}else{
		std::cout<<"Socket::bindAddr() bind addr : "<<addr.getInfo()<<std::endl;
	}
}

void Socket::listen(){
	std::cout<<"Socket::listen()"<<std::endl;
	int ret=::listen(socketFd_,maxListenListNum);
	if(ret==-1){
		std::cerr<<"Socket::listen() error"<<std::endl;
	}
}

int Socket::accept(InetAddress* peeraddr){
	struct sockaddr_in addr;
	memset(&addr,0,sizeof addr);
	socklen_t addr_len=static_cast<socklen_t>(sizeof addr);
	int connfd=::accept(socketFd_,(struct sockaddr*)(&addr),&addr_len);
	std::cout<<"Socket::accetp() : accetp connect from "<<addr.sin_addr.s_addr<<" : "<<ntohs(addr.sin_port)<<std::endl;
	//if(connfd==-1){
	//	std::cerr<<"Socket::accept() error"<<std::endl;
	//}
	if(connfd>=0){
		peeraddr->setAddr(addr);
	}
	//peeraddr->setAddr(addr);
	return connfd;
}

void Socket::shutdownWrite(){
	int ret=::shutdown(socketFd_,SHUT_WR);
	if(ret<0){
		std::cerr<<"Socket::shutdownWrite() error"<<std::endl;
	}
}

void Socket::setTcpNoDelay(bool on){
	int optval=on?1:0;
	::setsockopt(socketFd_,IPPROTO_TCP,TCP_NODELAY,&optval,sizeof optval);
}