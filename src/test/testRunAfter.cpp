#include"EventLoop.h"

#include<iostream>
#include<string>

int cnt=0;
server::EventLoop* g_loop;

void printfTid(){
	std::cout<<"pid = "<<getpid()<<" , tid = "<<pthread_self()<<std::endl;
	std::cout<<"now "<<Timestamp::now().get()<<std::endl;
}

void print(string str){
	std::cout<<"msg "<<Timestamp::now()<<" "<<str<<std::endl;
	if(++cnt==20){
		g_loop->quit();
	}
}

int main(){
	printfTid();
	server::EventLoop loop;
	g_loop=loop;

	print("main");
	loop.runAfter(1,bind(print,"once1"));
	loop.runAfter(1.5,bind(print,"once1.5"));
	loop.runAfter(2.5,bind(print,"once2.5"));
	loop.runAfter(3.5,bind(print,"once3.5"));
	loop.runEvery(2,bind(print,"every2"));
	loop.runEvery(3,bind(print,"every3"));

	loop.loop();
	print("main loop exits");
	sleep(1);
}