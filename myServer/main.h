#include"EventLoop.h"

EventLoop* g_loop;

int main(){
	EventLoop loop;
	g_loop=&loop;
	
}