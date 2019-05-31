#include"EventLoop.h"
#include<iostream>

EventLoop* g_loop;
int flag=0;

void run3(){
	std::cout<<"run3() pid = "<<getpid()<<" , flag = "<<flag<<std::endl;
	g_loop->quit();
}

void run2(){
	std::cout<<"run2() pid = "<<getpid()<<" , flag = "<<flag<<std::endl;
	flag=2;
	g_loop->runInLoop(run3);
}

void run1(){
	std::cout<<"run1() pid = "<<getpid()<<" , flag = "<<flag<<std::endl;
	flag=1;
	g_loop->runInLoop(run2);
}

int main(){
	std::cout<<"main() pid = "<<getpid()<<" , flag = "<<flag<<std::endl;
	EventLoop loop;
	g_loop=&loop;
	g_loop->runInLoop(run1);
	loop.loop();
}