#include"../Timer.h"
#include"../EventLoop.h"
#include"../base/Timestamp.h"
#include<iostream>
#include<pthread.h>

using namespace server;

int cnt=0;
EventLoop* g_loop;

void print(const string& str){
	std::cout<<str<<std::endl;
	if(++cnt==20){
		g_loop.quit();
	}
}

int main(){
	std::cout<<"pid = "<<getpid()<<" , tid = "<<pthread_self()<<std::endl;
	std::cout<<"now = "<< Timestamp::now().get()<<std::endl;
	sleep(1);
	{
		EventLoop loop;
		g_loop=&loop;
		loop.runAfter(1,std::bind(print,"once1"));
	}
}