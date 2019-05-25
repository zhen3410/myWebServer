#include"EventLoop.h"
#include"base/Thread.h"

#include<iostream>
#include<unistd.h>

EventLoop* g_loop;

void ThreadFunc(){
	//g_loop->loop();
	std::cout<<"threadFunc() pid = "<<getpid()<<" , tid = "<<CurrentThread::tid()<<std::endl;
	EventLoop loop;
	loop.loop();
}

int main(){
	std::cout<<"main() pid = "<<getpid()<<" , tid = "<<CurrentThread::tid()<<std::endl;
	EventLoop loop;
	EventLoop loop1;
	//g_loop=&loop;
	Thread t(ThreadFunc,"Thread1");
	t.start();
	loop.loop();
	return 0;
}
