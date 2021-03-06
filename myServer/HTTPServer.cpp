#include"HTTPServer.h"
#include"TcpConnection.h"

#include<iostream>

HTTPServer::HTTPServer(EventLoop& loop,int port,const std::string& name)
    :loop_(loop),
    server_(loop_,port,name)
{
    server_.setConnectionCallBack(std::bind(&HTTPServer::onConnection,this,std::placeholders::_1));
    server_.setMessageCallBack(std::bind(&HTTPServer::onMessage,this,std::placeholders::_1));
}

HTTPServer::~HTTPServer(){

}

void HTTPServer::start(){
    server_.start();
}

void HTTPServer::onConnection(const TcpConnectionPtr& conn){

}

void HTTPServer::onMessage(const TcpConnectionPtr& conn){
    //std::cout<<"HTTPServer::onMessage() get a http cnnection , and produce response"<<std::endl;
    HTTPRequest* request=conn->getHTTPRequest();
    HTTPResponse* response=conn->getHTTPResponse();
    bool ok=request->ParseRequest(conn->getInputBuffer());
    if(!ok){
        //解析不成功，返回400错误
        response->httpError("400 Bad Request");
    }else{
        //解析成功，产生响应
        response->processRequest(request);
    }
    //std::cout<<"HTTPServer::onMessage() send to client response is = "<<response->getResponse()<<std::endl;
    conn->send(response->getResponse());
    std::string close=request->getHeader("Connection");
    if(close=="close"){
        conn->ConnectionClosed();
    }
}
