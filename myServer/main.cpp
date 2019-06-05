#include"EventLoop.h"
#include"HTTPServer.h"

#include<iostream>
#include<memory>
#include<sys/socket.h>
#include<netinet/in.h>
#include<string.h>

int main(){
	std::cout<<"main() pid = "<<getpid()<<" , tid = "<<CurrentThread::tid()<<std::endl;
	
	EventLoop loop;
	HTTPServer server(loop,80,"ZhangZhen");
	server.setThreadNum(4);
	server.start();

	loop.loop();

}
