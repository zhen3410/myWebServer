#ifndef SERVER_CHANNEL_H
#define SERVER_CHANNEL_H

#include"EventLoop.h"

#include<functional>
#include<memory>


class Channel : public std::enable_shared_from_this<Channel>{

public:

	typedef std::function<void()> CallBack;

	Channel(const Channel&)=delete;
	void operator=(const Channel&)=delete;

	Channel(EventLoop& loop,int fd);
	~Channel();

	void enableReading(){
		events_|=kReadEvent;
		//setET();
		//update();
	}
	void enableWriting(){
		events_|=kWriteEvent;
		//setET();
		//update();
	}
	void disableReading(){
		events_&=~kReadEvent;
		//update();
	}
	void disableWriting(){
		events_&=~kWriteEvent;
		//update();
	}
	void disableAll(){
		events_=kNoneEvent;
		//update();
	}

	void setET(){
		events_|=kEdgeTrigger;
	}

	void addChannel(){
		loop_.addChannel(shared_from_this());
	}

	void updateChannel(){
		loop_.updateChannel(shared_from_this());
	}

	void delChannel(){
		loop_.removeChannel(shared_from_this());
	}

	void setRevents(int revent){
		revents_=revent;
	}
	void setReadCallBack(const CallBack& cb){
		readCallBack_=cb;
	}
	void setWriteCallBack(const CallBack& cb){
		writeCallBack_=cb;
	}
	void setErrorCallBack(const CallBack& cb){
		errorCallBack_=cb;
	}
	void setCloseCallBack(const CallBack& cb){
		closeCallBack_=cb;
	}

	void handleEvent();

	int fd()const{return fd_;}
	int events()const{return events_;}

private:

	static const int kNoneEvent;
	static const int kReadEvent;
	static const int kWriteEvent;
	static const int kEdgeTrigger;

	CallBack readCallBack_;
	CallBack writeCallBack_;
	CallBack errorCallBack_;
	CallBack closeCallBack_;

	EventLoop& loop_;
	const int fd_;

	int events_;
	int revents_;

};


#endif
