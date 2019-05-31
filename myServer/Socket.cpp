#include"Socket.h"

#include<sys/socket.h>
#include<netinet/in.h>
#include<fcntl.h>
#include<assert.h>
#include<unistd.h>
#include<memory>
#include<string.h>

void setNonBlockAndCloseOnExec(int fd){
    int flags=fcntl(fd,F_GETFL,0);
    flags|=O_NONBLOCK;
    int ret=fcntl(fd,F_SETFL,flags);
    assert(ret>=0);

    flags=fcntl(fd,F_GETFD,0);
    flags|=FD_CLOEXEC;
    ret=fcntl(fd,F_SETFD,flags);
    assert(ret>=0);
}

Socket::Socket(int port)
    :closed_(false),
    socketFd_(socket(AF_INET,SOCK_STREAM,0)),
    port_(port)
{
    assert(socketFd_>=0);
    setNonBlockAndCloseOnExec(socketFd_);
}

Socket::~Socket(){
    if(!closed_)
        ::close(socketFd_);
}

void Socket::bindAndListening(){
    struct sockaddr_in servaddr;
    memset(&servaddr,0,sizeof servaddr);
    servaddr.sin_family=AF_INET;
    servaddr.sin_addr.s_addr=htonl(INADDR_ANY);
    servaddr.sin_port=htons(port_);
    int ret=bind(socketFd_,(const struct sockaddr*)(&servaddr),(socklen_t)(sizeof servaddr));
    assert(ret==0);
    ret=listen(socketFd_,SOMAXCONN);
    assert(ret==0);
}

int Socket::accept(struct sockaddr_in& acAddr){
    int connfd=::accept(socketFd_,(struct sockaddr*)(&acAddr),(socklen_t*)(sizeof acAddr));
    assert(connfd>=0);
    setNonBlockAndCloseOnExec(connfd);
    return connfd;
}

void Socket::close(){
    ::close(socketFd_);
    closed_=true;
}
