#ifndef SERVER_CHANNEL_H
#define SERVER_CHANNEL_H

#include<functional>

namespace server{

class EventLoop;

class Channel{
public:

	Channel(const Channel&)=delete;
	void operator=(const Channel&)=delete;

	typedef std::function<void()> EventCallBack;

	Channel(EventLoop* loop,int fd);
	~Channel();

	void handleEvent();

	void setReadCallback(const EventCallBack& cb){
		readCallback_=cb;
	}
	void setWriteCallback(const EventCallBack& cb){
		writeCallback_=cb;
	}
	void setErrorCallback(const EventCallBack& cb){
		errorCallback_=cb;
	}

	EventLoop* ownerLoop()const{return loop_;}

	int fd()const{return fd_;}
	int events()const{return events_;}
	int index()const{return index_;}
	void setRevents(int revt){revents_=revt;}
	void setIndex(int ind){index_=ind;}

	bool isNoneEvent(){return events_==kNoneEvent;}

	void enableReading(){events_|=kReadEvent;update();}
	void disableReading(){events_&=~kReadEvent;update();}
	void enableWriting(){events_|=kWriteEvent;update();}
	void disableWriting(){events_&=~kWriteEvent;update();}
	void disableAll(){events_=kNoneEvent;update();}

	void remove();

private:
	void update();

	static const int kNoneEvent;
	static const int kReadEvent;
	static const int kWriteEvent;

	EventLoop* loop_;
	const int fd_;
	int events_;
	int revents_;
	int index_;

	bool eventHandling_;
	bool addedToLoop_;
	EventCallBack readCallback_;
	EventCallBack writeCallback_;
	EventCallBack errorCallback_;
};

}

#endif
