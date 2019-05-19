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
class EventLoopThreadPool;

class TcpServer{
public:
	typedef std::shared_ptr<TcpConnection> TcpConnectionPtr;
	typedef std::function<void(const TcpConnectionPtr&)> ConnectionCallBack;
	typedef std::function<void(const TcpConnectionPtr&)> WriteCompleteCallBack;
	typedef std::function<void(const TcpConnectionPtr&,Buffer*,Timestamp)> MessageCallBack;


	TcpServer(const TcpServer&)=delete;
	void operator=(const TcpServer&)=delete;

	TcpServer(EventLoop*,const InetAddress&,std::string);
	~TcpServer();

	void setThreadNum(int numThreads);
	
	void start();
	void setConnectionCallBack(const ConnectionCallBack& cb){
		connectionCallBack_=cb;
	}
	void setMessageCallBack(const MessageCallBack& cb){
		messageCallBack_=cb;
	}
	void setWriteCompleteCallBack(const WriteCompleteCallBack& cb){
		writeCompleteCallBack_=cb;
	}

private:
	void newConnection(int sockfd,const InetAddress& peeraddr);
	void removeConnection(const TcpConnectionPtr&);
	void removeConnectionInLoop(const TcpConnectionPtr& conn);

	typedef std::map<std::string,TcpConnectionPtr> ConnectionMap;

	EventLoop* loop_;
	const std::string name_;
	std::unique_ptr<Acceptor> acceptor_;
	std::unique_ptr<EventLoopThreadPool> threadPool_;
	ConnectionCallBack connectionCallBack_;
	MessageCallBack messageCallBack_;
	WriteCompleteCallBack writeCompleteCallBack_;
	bool started_;
	int nextConnId_;
	ConnectionMap connections_;
};

}



#endif
