#ifndef SERVER_HTTPSERVER_H
#define SERVER_HTTPSERVER_H

#include"TcpServer.h"
#include"EventLoopThreadPool.h"

#include<string>
#include<memory>

class EventLoop;
class TcpConnection;

class HTTPServer
{
public:
    typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
    HTTPServer(EventLoop& loop,int port,const std::string&);
    ~HTTPServer();

    void start();
    void setThreadNum(int num){
        server_.setThreadNum(num);
    }

    void onConnection(const TcpConnectionPtr& conn);
    void onMessage(const TcpConnectionPtr& conn);

private:
    /* data */
    EventLoop& loop_;
    TcpServer server_;
};


#endif