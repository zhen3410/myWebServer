#ifndef SERVER_TCPCONNECTION_H
#define SERVER_TCPCONNECTION_H

#include"EventLoop.h"

#include<memory>
#include<netinet/in.h>
#include<string>
#include<functional>

class TcpConnection : public std::enable_shared_from_this<TcpConnection>
{
public:
    typedef std::function<void()> ConnectionCallBack;
    typedef std::function<void()> MessageCallBack;
    typedef std::function<void(const std::string&)> CloseCallBack;

    TcpConnection(const TcpConnection&)=delete;
    void operator=(const TcpConnection&)=delete;

    TcpConnection(EventLoop& loop,int fd,const struct sockaddr_in& peerAddr,const std::string& name);
    ~TcpConnection();

    void setConnectionCallBack(const ConnectionCallBack& cb){
        connectionCallBack_=cb;
    }
    void setMessageCallBack(const MessageCallBack& cb){
        messageCallBack_=cb;
    }
    void setCloseCallBack(const CloseCallBack& cb){
        closeCallBack_=cb;
    }

    void connectionDestroy();


private:
    void readHandle();
    void writeHandle();
    void errorHandle();
    void closeHandle();

    /* data */
    EventLoop& loop_;
    int fd_;
    struct sockaddr_in peerAddr_;
    const std::string name_;

    std::shared_ptr<Channel> channelPtr_;

    ConnectionCallBack connectionCallBack_;
    MessageCallBack messageCallBack_;
    CloseCallBack closeCallBack_;
};



#endif
