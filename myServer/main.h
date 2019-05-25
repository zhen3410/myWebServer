#include"EventLoop.h"

EventLoop* g_loop;

void ThreadFunc(){
	g_loop->loop();
}

int main(){
	EventLoop loop;
	g_loop=&loop;
	Thread t(ThreadFunc,"Thread1");
	t.start();
	t.join();
}