test3:test3.o EventLoop.o channel.o Poller.o
	g++ test3.o EventLoop.o channel.o Poller.o -lpthread -g -o test3
test3.o:test3.cpp
	g++ -std=c++11 -g -c test3.cpp
EventLoop.o:EventLoop.cpp EventLoop.h
	g++ -std=c++11 -g -c EventLoop.cpp
channel.o:channel.cpp channel.h
	g++ -std=c++11 -g -c channel.cpp
Poller.o:Poller.cpp Poller.h
	g++ -std=c++11 -g -c Poller.cpp
clean:
	rm -rf *.o test3
