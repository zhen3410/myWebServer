#ifndef SERVER_CONNECTOR_H
#define SERVER_CONNECTOR_H

namespace server{

class Channel;
class EventLoop;

class Connector{

public:
	Connector(const Connector&)=delete;
	void operator=(const Connector&)=delete;

	typedef std::function<void(int)> NewConnectionCallback;
	typedef std::shared_ptr<Connector> ConnectorPtr;

	Connector(EventLoop* loop,const InetAddress& serverAddr);
	~Connector();

	void setNewConnectionCallBack(const NewConnectionCallback& cb){
		newConnectionCallBack_=cb;
	}

	void start();
	void restart();
	void stop();

	const InetAddress& serverAddress()const{return serverAddr_;}

private:
	enum States{kDisconnected,kConnecting,kConnected,};
	static const int kMaxRetryDelaysMs=30*1000;
	static const int kInitRetryDelayMs=500;

	void setState(States s){
		state_=s;
	}
	void startInLoop();
	void connect();
	void connecting();
	void handleWrite();
	void handleError();
	void retry(int);
	int removeAndResetChannel();
	void resetChannel();

	EventLoop* loop_;
	InetAddress serverAddr_;
	bool connect_;
	States state_;
	std::unique_ptr<Channel> channel_;
	NewConnectionCallback newConnectionCallBack_;
	int retryDelayMs_;
	TimerId timerId_;


}

}

#endif