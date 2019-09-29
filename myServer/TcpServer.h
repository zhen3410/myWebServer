#ifndef SERVER_TCPSERVER_H
#define SERVER_TCPSERVER_H

#include"EventLoop.h"
#include"Socket.h"
#include"TimingWheel.h"
#include"EventLoopThreadPool.h"
#include"TcpConnection.h"

#include<memory>
#include<map>
#include<functional>
#include<string>



class Channel;
class EventLoopThreadPool;

class TcpServer
{
public:
    typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
    typedef std::function<void(const TcpConnectionPtr&)> ConnectionCallBack;
    typedef std::function<void(const TcpConnectionPtr&)> MessageCallBack;
    typedef std::function<void(const std::string&)> CloseCallBack;

    TcpServer(const TcpServer&)=delete;
    void operator=(const TcpServer&)=delete;

    TcpServer(EventLoop&,int port,const std::string& name);
    ~TcpServer();

    void start();

    void setThreadNum(int num){
        threadPool_->setThreadNum(num);
    }

    void setConnectionCallBack(const ConnectionCallBack& cb){
        connectionCallBack_=cb;
    }
    void setMessageCallBack(const MessageCallBack& cb){
        messageCallBack_=cb;
    }
    void ConnectionCloseCallBack(const std::string&);
    void ConnectionCloseCallBackInLoop(const std::string&);
    void newConnection();
    void ConnMessageCallBack(const TcpConnectionPtr&);

private:
    /* data */
    EventLoop& loop_;
    Socket socket_;
    std::string name_;

    std::shared_ptr<Channel> acceptChannel_;

    TimingWheel timingWheel_;

    int ConnectionId_;

    ConnectionCallBack connectionCallBack_;
    MessageCallBack messageCallBack_;
    CloseCallBack ConnectionCloseCallBack_;

    std::map<std::string,TcpConnectionPtr> conn_;
    std::unique_ptr<EventLoopThreadPool> threadPool_;

    const static int maxConn_=1020;
};



#endif
