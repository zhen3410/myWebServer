#include"TcpServer.h"
#include"EventLoop.h"
#include"InetAddress.h"
#include"TcpConnection.h"

#include<iostream>
#include<unistd.h>
#include<string>
#include<bitset>
#include<stdio.h>

//EventLoop* g_loop;
void onConnection(const server::TcpConnection::TcpConnectionPtr& conn){
	if(conn->connected()){
		std::cout<<"onConnection(): new connection ["<<conn->getName()<<"] from "
		<<conn->getPeerAddress().getInfo()<<std::endl;
	}else{
		std::cout<<"onConnection(): connection ["<<conn->getName()<<"] is down."<<std::endl;
	}
}

void onMessage(const server::TcpConnection::TcpConnectionPtr& conn,
			   server::Buffer* buf,
			   server::Timestamp receiveTime){
	std::cout<<"onMessage(): received "<<buf->readableBytes()<<" bytes from connection["<<conn->getName()
	<<"] at "<<receiveTime.get()<<std::endl;
	std::string str=buf->retrieveAsString();
	std::cout<<"onMessage(): "<<str<<std::endl;
	conn->send(str);
}

int main(){

	std::cout<<"main() : pid = "<<getpid()<<std::endl;
	
	server::InetAddress listenAddr(9981);
	server::EventLoop loop;
	//g_loop=&loop;

	server::TcpServer server(&loop,listenAddr,"FirstServer");
	server.setConnectionCallBack(onConnection);
	server.setMessageCallBack(onMessage);
	server.start();

	loop.loop();

	return 0;
}
