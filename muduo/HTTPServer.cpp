#include"HTTPServer.h"

#include"HTTPContext.h"
#include"HTTPRequest.h"
#include"HTTPResponse.h"

#include<iostream>
#include<any>

using namespace server;

namespace {
    void defaultHttpCallBack(const HTTPRequest&,HTTPResponse* resp){
        resp->setStatusCode(HTTPResponse::k404Found);
        resp->setStatusMessgae("Not Found");
        resp->setCloseConnection(true);
    }
}

HTTPServer::HTTPServer(EventLoop* loop,const InetAddress& listenAddr,const std::string& name)
    :server_(loop,listenAddr,name),
    httpCallBack_(defaultHttpCallBack)
{
    server_.setConnectionCallBack(std::bind(&HTTPServer::onConnection,this,_1));
    server_.setMessageCallBack(std::bind(&HTTPServer::onMessage,this,_1,_2));
}

void HTTPServer::start(){
    std::cout<<"HttpServer ["<<name<<"] starts"<<std::endl;
    server_.start();
}

void HTTPServer::onConnection(const HTTPServer::TcpConnectionPtr& conn){
    if(conn->connected()){
        conn->setContext(HTTPContext());
    }
}

void HTTPServer::onMessage(const HTTPServer::TcpConnectionPtr& conn,Buffer* buf,Timestamp receiveTime){
    HTTPContext* context=std::any_cast<HTTPContext>(conn->getMutableContext());
    if(!context->parseRequest(buf,receiveTime)){
        conn->send("HTTP/1.1 400 Bad Request\r\n\r\n");
        conn->shutdown();
    }

    if(context->gotAll()){
        onRequest(conn,context->request());
        context->reset();
    }
}

void HTTPServer::onRequest(const TcpConnectionPtr& conn,const HTTPRequest& req){
    const string& connection=req.getHeader("Connection");
    bool close=connection=="close"||(req.getVersion()==HTTPRequest::kHttp10&&connection!="Keep-Alive");
    HTTPContext response(close);
    httpCallBack_(req,&response);
    Buffer buf;
    response.appendToBuffer(&buf);
    conn->send(&buf);
    if(response.closeConnection()){
        conn->shutdown();
    }
}