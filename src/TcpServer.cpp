#include"TcpServer.h"

using namespace server;

TcpServer::TcpServer(EventLoop* loop,const InetAddress& listenAddr,string name)
	:loop_(loop),
	name_(name),
	acceptor_(new Acceptor(loop_,listenAddr)),
	started_(false),
	nextConnId_(1)
{
	acceptor_->setNewConnectionCallBack(std::bind(&TcpServer::newConnection,this));
}

TcpServer::~TcpServer(){

}

void TcpServer::start(){
	if(!start){
		started_=true;
	}

	if(!acceptor_->listenning()){
		loop_->runInLoop(std::bind(&Acceptor::listen,acceptor_.get()));
	}
}

void TcpServer::newConnection(int sockfd,const InetAddress& peerAddr){
	loop_->assertInLoopThread();
	std::string connName=name_+std::to_string(nextConnId_++);
	std::cout<<"TcpServer::newConnection ["<<name_<<"] - new connection ["
	<<connName<<"] from "<<peerAddr.getInfo()<<std::endl;

	struct sockaddr_in localAddr_in;
	getsockname(sockfd,(struct sockaddr*)(&localAddr_in),sizeof sockaddr_in);
	InetAddress loaclAddr(localAddr_in);

	TcpConnectionPtr conn(make_shared<TcpConnection>(loop_,connName,sockfd,loaclAddr,peerAddr));
	conn->setConnectionCallBack(connectionCallBack_);
	conn->setMessageCallBack(messageCallBack_);
	conn->connectEstablished();

}