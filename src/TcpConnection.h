#ifndef SERVER_TCPCONNECTION_H
#define SERVER_TCPCONNECTION_H

#include"InetAddress.h"
#include"Buffer.h"

#include<functional>
#include<string>
#include<memory>

namespace server{

class EventLoop;
class Channel;
class Socket;
class Timestamp;

class TcpConnection:
	public std::enable_shared_from_this<TcpConnection>
{
public:

	typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
	typedef std::function<void(const TcpConnectionPtr&)> ConnectionCallBack;
	typedef std::function<void(const TcpConnectionPtr&)> CloseCallBack;
	//typedef std::function<void(const TcpConnectionPtr&,const char*,int)> MessageCallBack;
	typedef std::function<void(const TcpConnectionPtr&,Buffer*,Timestamp)> MessageCallBack;


	TcpConnection(const TcpConnection&)=delete;
	void operator=(const TcpConnection&)=delete;

	TcpConnection(EventLoop* loop,
				  std::string name,
		 		  int sockfd,
		 		  const InetAddress& localAddr,
		 		  const InetAddress& peerAddr);
	~TcpConnection();

	const std::string& getName()const{return name_;}
	EventLoop* getLoop()const{return loop_;}
	const InetAddress& getLocalAddresss()const{return localAddr_;}
	const InetAddress& getPeerAddress()const{return peerAddr_;}


	bool connected()const{return state_==kConnected;}

	void setConnectionCallBack(const ConnectionCallBack& cb){
		connectionCallBack_=cb;
	}

	void setMessageCallBack(const MessageCallBack& cb){
		messageCallBack_=cb;
	}

	void setCloseCallBack(const CloseCallBack& cb){
		closeCallBack_=cb;
	}

	void connectionEstablished();
	void connectionDestroyed();

private:
	enum StateE{kConnecting,kConnected,};

	void setState(StateE s){state_=s;}
	void handleRead(Timestamp);
	void handleClose();

	EventLoop* loop_;
	std::string name_;
	StateE state_;

	std::unique_ptr<Socket> socket_;
	std::unique_ptr<Channel> channel_;
	InetAddress localAddr_;
	InetAddress peerAddr_;
	ConnectionCallBack connectionCallBack_;
	MessageCallBack messageCallBack_;
	CloseCallBack closeCallBack_;
	Buffer inputBuffer_;
};

}



#endif
