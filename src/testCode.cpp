#include"../EventLoop.h"
#include"../EventLoopThread.h"
#include<iostream>

void runInThread(){
	std::cout<<"runInThread() : pid = "<<getpid()<<" , tid = "<<pthread_self()<<std::endl;
}

int main(){
	std::cout<<"main() : pid = "<<getpid()<<" , tid = "<<pthread_self()<<std::endl;

	server::EventLoopThread loopThread;
	server::EventLoop* loop=loopThread.startLoop();
	loop->runInLoop(runInThread);
	sleep(1);
	loop->runAfter(2,runInThread);
	sleep(3);
	loop->quit();

	std::cout<<"exit main."<<std::endl;
}