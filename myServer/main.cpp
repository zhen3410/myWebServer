#include"EventLoop.h"
#include"EventLoopThread.h"
#include<iostream>

void runInThread(){
	std::cout<<"runInThrad(): pid = "<<getpid()<<" , tid = "<<CurrentThread::tid()<<std::endl;
}

int main(){
	std::cout<<"main() pid = "<<getpid()<<" , tid = "<<CurrentThread::tid()<<std::endl;
	EventLoopThread loopThread;
	EventLoop* loop=loopThread.startLoop();
	loop->runInLoop(runInThread);
	loop->quit();
	std::cout<<"main() pid = "<<getpid()<<" , flag = "<<flag<<std::endl;
}