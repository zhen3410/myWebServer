#include"EventLoop.h"
#include"EventLoopThread.h"
#include"Channel.h"
#include"Socket.h"
#include"TcpServer.h"
#include"TcpConnection.h"

#include<iostream>
#include<memory>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>

Socket* g_socket;

void newConnection(){
	struct sockaddr_in peerAddr;
	memset(&peerAddr,0,sizeof peerAddr);
	int connfd=g_socket->accept(peerAddr);
	std::cout<<"newConnection() accept a new Connection from "
		<<ntohl(peerAddr.sin_addr.s_addr)<<":"<<ntohs(peerAddr.sin_port)<<std::endl;
	write(connfd,"How are you?\n",13);
	g_socket->close();
}

void onConnection(std::shared_ptr<TcpConnection> conn){

}

void onMessage(std::shared_ptr<TcpConnection> conn){
	conn->send("hello new server");
}

int main(){
	std::cout<<"main() pid = "<<getpid()<<" , tid = "<<CurrentThread::tid()<<std::endl;
	
	EventLoop loop;
	TcpServer server(loop,9981,"Server");
	server.setConnectionCallBack(onConnection);
	server.setMessageCallBack(onMessage);
	server.start();

	loop.loop();

}
