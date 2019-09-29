#include"EventLoop.h"
#include"HTTPServer.h"
#include"TcpConnection.h"

#include<iostream>
#include<memory>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>

void onMessage(const std::shared_ptr<TcpConnection>& conn){
	conn->inputBufferClear();
	conn->send("hello onMessage");
}

void onConnection(const std::shared_ptr<TcpConnection>& conn){
	conn->send("hello onConnection");
}


int main(){
	//std::cout<<"main() pid = "<<getpid()<<" , tid = "<<CurrentThread::tid()<<std::endl;
	
	
	EventLoop loop;
	HTTPServer server(loop,80,"ZhangZhen");
	server.setThreadNum(3);
	server.start();

	loop.loop();

	/*
	EventLoop loop;
	TcpServer server(loop,6666,"zhangzhen");
	server.setThreadNum(4);
	server.setMessageCallBack(onMessage);
	server.setConnectionCallBack(onConnection);
	server.start();

	loop.loop();
	 */
}
