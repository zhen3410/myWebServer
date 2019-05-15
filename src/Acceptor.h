#ifndef SERVER_ACCEPTOR_H
#define SERVER_ACCEPTOR_H

#include"channel.h"
#include"Socket.h"

#include<functional>

namespace server{

class EventLoop;
class InetAddress;

class Acceptor{

public:
	typedef std::function<void(int,const InetAddress&)> NewConnectionCallBack;

	Acceptor(const Acceptor&)=delete;
	void operator=(const Acceptor&)=delete;

	Acceptor(EventLoop* loop,const InetAddress& listenAddr);
	~Acceptor();

	void setNewConnectionCallBack(const NewConnectionCallBack& cb){
		newConnectionCallBack_=cb;
	}

	bool listening()const{return listening_;}

	void listen();


private:
	void handleRead();

	EvnetLoop* loop_;
	Socket acceptSocket_;
	Channel acceptChannel_;
	NewConnectionCallBack newConnectionCallBack_;
	bool listening_;
	int idleFd_;

}

}


#endif