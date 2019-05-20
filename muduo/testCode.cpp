#include"TcpServer.h"
#include"EventLoop.h"
#include"InetAddress.h"
#include"TcpConnection.h"

#include<iostream>
#include<unistd.h>
#include<string>
#include<bitset>
#include<stdio.h>

std::string message1;
std::string message2;

//EventLoop* g_loop;
void onConnection(const server::TcpConnection::TcpConnectionPtr& conn){
	if(conn->connected()){
		std::cout<<"onConnection(): new connection ["<<conn->getName()<<"] from "
			<<conn->getPeerAddress().getInfo()<<std::endl;
		conn->send(message1);
		//conn->send(message2);
		//conn->shutdown();
	}else{
		std::cout<<"onConnection(): connection ["<<conn->getName()<<"] is down."<<std::endl;
	}
}

void onWriteComplete(const server::TcpConnection::TcpConnectionPtr& conn){
	std::cout<<"OnWriteComplete send somthing = ["<<message1<<"]"<<std::endl;
	conn->send(message1);
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

	message1=std::string(10,'a');
	message2=std::string(500,'b');

	server::InetAddress listenAddr(9981);
	server::EventLoop loop;
	//g_loop=&loop;
	//
														
	server::TcpServer server(&loop,listenAddr,"FirstServer");
	server.setConnectionCallBack(onConnection);
	server.setMessageCallBack(onMessage);
	//server.setWriteCompleteCallBack(onWriteComplete);
	server.setThreadNum(5);
	server.start();
	loop.loop();
	return 0;
}

