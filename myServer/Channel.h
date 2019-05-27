#ifndef SERVER_CHANNEL_H
#define SERVER_CHANNEL_H

#include<functional>
#include<memory>

class EventLoop;

class Channel : std::enable_shared_from_this<Channel>{

public:

	typedef std::function<void()> CallBack;

	Channel(const Channel&)=delete;
	void operator=(const Channel&)=delete;

	Channel(EventLoop& loop,int fd);
	~Channel();

	void setEvents(int event){
		events_=event;
	}

	void enableReading(){
		events_|=kReadEvent;
		update();
	}
	void enableWriting(){
		events_|=kWriteEvent;
		update();
	}
	void disableReading(){
		events_&=~kReadEvent;
		update();
	}
	void disableWriting(){
		events_&=~kWriteEvent;
		update();
	}
	void disableAll(){
		events_=kNoneEvent;
		update();
	}

	void setRevents(int revent){
		revents_=revent;
	}

	void setReadCallBack(CallBack& cb){
		readCallBack_=cb;
	}
	void setWriteCallBack(CallBack& cb){
		writeCallBack_=cb;
	}
	void setErrorCallBack(CallBack& cb){
		errorCallBack_=cb;
	}

	void handleEvent();

	int fd()const{return fd_;}
	int events()const{return events_;}

private:
	void update(){
		loop_.updateChannel(std::shared_from_this());
	}

	static const int kNonoEvent;
	static const int kReadEvent;
	static const inr kWriteEvent;

	CallBack readCallBack_;
	CallBack writeCallBack_;
	CallBack errorCallBack_;

	EventLoop& loop_;
	const int fd_;

	int events_;
	int revents_;

};


#endif