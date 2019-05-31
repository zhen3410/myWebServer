#ifndef SERVER_TCPCONNECTION_H
#define SERVER_TCPCONNECTION_H

#include"EventLoop.h"

#include<memory>
#include<netinet/in.h>
#include<functional>

class TcpConnection
{
public:
    typedef std::function<void()> ConnectionCallBack;
    typedef std::function<void()> MessageCallBack;
    typedef std::function<void()> ReadHandle;
    typedef std::function<void()> WriteHandle;
    typedef std::function<void()> ErrorHandle;

    TcpConnection(const TcpConnection&)=delete;
    void operator=(const TcpConnection&)=delete;

    TcpConnection(EventLoop& loop,int fd,const struct sockaddr_in& peerAddr);
    ~TcpConnection();

    void setConnectionCallBack(const ConnectionCallBack& cb){
        connectionCallBack_=cb;
    }
    void setMessageCallBack(const MessageCallBack& cb){
        messageCallBack_=cb;
    }


private:
    void readHandle();
    void writeHandle();
    void errorHandle();

    /* data */
    EventLoop& loop_;
    int fd_;
    struct sockaddr_in peerAddr_;

    std::shared_ptr<Channel> channelPtr_;

    ConnectionCallBack connectionCallBack_;
    MessageCallBack messageCallBack_;

};



#endif
