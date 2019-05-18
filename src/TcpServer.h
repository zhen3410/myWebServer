#ifndef SERVER_TCPSERVER_H
#define SERVER_TCPSERVER_H

#include<string>
#include<functional>
#include<map>
#include<memory>


namespace server{

class EventLoop;
class Acceptor;
class TcpConnection;
class Buffer;
class Timestamp;
class InetAddress;

class TcpServer{
public:
	typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
	typedef std::function<void(const TcpConnectionPtr&)> ConnectionCallBack;
	typedef std::function<void(const TcpConnectionPtr&,Buffer*,Timestamp)> MessageCallBack;

	TcpServer(const TcpServer&)=delete;
	void operator=(const TcpServer&)=delete;

	TcpServer(EventLoop*,const InetAddress&,std::string);
	~TcpServer();

	void start();
	void setConnectionCallBack(const ConnectionCallBack& cb){
		connectionCallBack_=cb;
	}
	void setMessageCallBack(const MessageCallBack& cb){
		messageCallBack_=cb;
	}


private:
	void newConnection(int sockfd,const InetAddress& peeraddr);
	void removeConnection(const TcpConnectionPtr&);

	typedef std::map<std::string,TcpConnectionPtr> ConnectionMap;

	EventLoop* loop_;
	const std::string name_;
	std::unique_ptr<Acceptor> acceptor_;
	ConnectionCallBack connectionCallBack_;
	MessageCallBack messageCallBack_;
	bool started_;
	int nextConnId_;
	ConnectionMap connections_;
};

}



#endif
