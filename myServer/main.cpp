#include"EventLoop.h"
#include"EventLoopThread.h"
#include"Channel.h"
#include"Socket.h"
#include"TcpServer.h"

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

int main(){
	std::cout<<"main() pid = "<<getpid()<<" , tid = "<<CurrentThread::tid()<<std::endl;
	
	EventLoop loop;
<<<<<<< HEAD
	TcpServer server(loop,9981,"Server");
	server.start();
=======
	Socket socket(9981);
	g_socket=&socket;
	std::shared_ptr<Channel> pCh(new Channel(loop,socket.fd()));
	pCh->enableReading();
	pCh->setReadCallBack(newConnection);
	socket.bindAndListening();
	
	std::cout<<"loop started"<<std::endl;
>>>>>>> d5d92ef8fa090a1a1642d960642224c4a6925f1e

	loop.loop();

}
