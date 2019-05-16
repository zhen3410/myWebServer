#include"TcpServer.h"
#include"EventLoop.h"
#include"InetAddress.h"
#include"TcpConnection.h"

#include<iostream>
#include<unistd.h>

void onConnection(const server::TcpConnection::TcpConnectionPtr& conn){
	if(conn->connected()){
		std::cout<<"onConnection(): new connection ["<<conn->getName()<<"] from "
		<<conn->getPeerAddress().getInfo()<<std::endl;
	}else{
		std::cout<<"onConnection(): connection ["<<conn->getName()<<"] is down."<<std::endl;
	}
}

void onMessage(const server::TcpConnection::TcpConnectionPtr& conn,
			   const char* data,
			   ssize_t len)
{
	std::cout<<"onMessage(): received "<<len<<" bytes from connection ["
	<<conn->getName()<<"]."<<std::endl;
}

int main(){

	std::cout<<"main() : pid = "<<getpid()<<std::endl;
	
	server::InetAddress listenAddr(9981);
	server::EventLoop loop;

	server::TcpServer server(&loop,listenAddr,"FirstServer");
	server.setConnectionCallBack(onConnection);
	server.setMessageCallBack(onMessage);
	server.start();
	
	std::cout<<"start loop"<<std::endl;
	loop.loop();

}
