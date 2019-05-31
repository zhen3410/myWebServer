#ifndef SERVER_TCPSERVER_H
#define SERVER_TCPSERVER_H

#include"EventLoop.h"
#include"Socket.h"

#include<memory>
#include<map>
#include<functional>
#include<string>



class TcpConnection;
class Channel;

class TcpServer
{
public:
    typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
    typedef std::function<void(TcpConnectionPtr)> ConnectionCallBack;
    typedef std::function<void(TcpConnectionPtr)> MessageCallBack;

    TcpServer(const TcpServer&)=delete;
    void operator=(const TcpServer&)=delete;

    TcpServer(const EventLoop&,int port,const std::string& name);
    ~TcpServer();

    void start();
    void setConnectionCallBack(const ConnectionCallBack& cb){
        connectionCallBack_=cb;
    }
    void setMessageCallBack(const MessageCallBack& cb){
        messageCallBack_=cb;
    }
    void newConnection();

private:
    /* data */
    EventLoop& loop;
    Socket socket_;
    std::string name_;

    std::shared_ptr<Channel> acceptChannel_;

    int ConnectionId_;

    ConnectionCallBack connectionCallBack_;
    MessageCallBack messageCallBack_;

    std::map<std::string,TcpConnectionPtr> conn_;
};



#endif