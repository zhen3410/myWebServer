#include"TcpServer.h"
#include"EventLoop.h"
#include"InetAddress.h"
#include<iostream>

void onConnection(const server::TcpConnection::TcpConnectionPtr& conn){
	if(conn->connected()){
		std::cout<<"onConnection(): new connection ["<<conn->name()<<"] from "
		<<conn->getPeerAddress().getInfo()<<std::endl;
	}else{
		std::cout<<"onConnection(): connection ["<<conn->name()<<"] is down."<<std::endl;
	}
}

void onMessage(const server::TcpConnection::TcpConnectionPtr& conn,
			   const char* data,
			   ssize_tlen)
{
	std::cout<<"onMessage(): received "<<len<<" bytes from connection ["
	<<conn->name()<<"]."<<std::endl;
}

int main(){

	std::cout<<"main() : pid = "<<getpid()<<std::endl;
	
	server::InetAddress listenAddr(9981);
	server::EventLoop loop;

	server::TcpServer server(&loop,listenAddr);
	server.setConnectionCallBack(onConnection);
	server.setMessageCallBack(onMessage);
	server.start();

	loop.loop();

}