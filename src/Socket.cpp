#include"Socket.h"
#include"InetAddress.h"

#include<sys/socket.h>
#include<iostream>

using namespace server;

static const int Socket::maxListenListNum=2048;

Socket::~Socket(){
	::close(socketFd_);
}


void Socket::setReuseAddr(bool flag){
	int optval=flag?1:0;
	::setsockopt(socketFd_,SOL_SOCKET,SO_REUSEADDR,&optval,
		static_cast<socklen_t>(sizeof optval));
}

void Socket::bindAddr(const InetAddress& addr){
	int ret=::bind(socketFd_,addr.get(),sizeof(struct sockaddr_in));
	if(ret==-1){
		std::cerr<<"Socket::bindAddr() error"<<std::endl;
	}
}

void Socket::listen(){
	int ret=::listen(socketFd_,maxListenListNum);
	if(ret==-1){
		std::cerr<<"Socket::listen() error"<<std::endl;
	}
}

int Socket::accept(InetAddress* peeraddr){
	struct sockaddr_in addr;
	memset(&addr,0,sizeof addr);
	int connfd=::accept(socketFd_,(struct sockaddr*)addr,NULL);
	if(connfd=-1){
		std::cerr<<"Socket::accept() error"<<std::endl;
	}
	peeraddr->setAddr(addr);
	return connfd;
}