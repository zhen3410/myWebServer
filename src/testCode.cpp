#include"EventLoop.h"
#include"EventLoopThread.h"
#include"Acceptor.h"
#include<iostream>
#include<unistd.h>

void runInThread(){
	std::cout<<"runInThread() : pid = "<<getpid()<<" , tid = "<<pthread_self()<<std::endl;
}

void newConnection(int sockfd,const server::InetAddress& peerAddr){
	std::cout<<"newConnection():accepted a new connection from "<<peerAddr.getInfo()<<std::endl;
	::write(sockfd,"How are you?\n",13);
	::close(sockfd);
}

int main(){
	std::cout<<"main() : pid = "<<getpid()<<" , tid = "<<pthread_self()<<std::endl;


	server::InetAddress listenAddr(9981);
	server::EventLoop loop;

	server::Acceptor acceptor(&loop,listenAddr);
	acceptor.setNewConnectionBack(newConnection);
	acceptor.listen();

	loop.loop();
	
	std::cout<<"exit main."<<std::endl;
}
