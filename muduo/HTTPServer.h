#ifndef SERVER_HTTPPSERVER_H
#define SERVER_HTTPSERVER_H

#include"TcpServer.h"

#include<functional>

namespace server{


class EventLoop;
class InetAddress;

class HTTPRequest;
class HTTPResponse;
class TcpConnection;

class HTTPServer
{
public:
    typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
    typedef std::function<void(const HTTPRequest&,HTTPResponse*)> HttpCallBack;

    HTTPServer(const HTTPServer&)=delete;
    void operator=(const HTTPServer&)=delete;

    HTTPServer(const EventLoop& loop,
               const std::string& name,
               const InetAddress& listenAddr);
    ~HTTPServer();

    void setHttpCallback(const HttpCallBack& cb){
        httpCallBack_=cb;
    }
    void setThreadNum(int numThreads){
        server_.setThreadNum(numThreads);
    }
    void start();
private:
    void onConnection(const TcpConnectionPtr& conn);
    void onMessage(const TcpConnectionPtr& conn,Buffer* buf,Timestamp receiveTime);
    void onRequest(const TcpConnectionPtr&,const HTTPRequest&);

    EventLoop& loop_;
    const std::string name;
    TcpServer server_;
    HttpCallBack httpCallBack_;
};

}




#endif