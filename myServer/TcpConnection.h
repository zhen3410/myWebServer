#ifndef SERVER_TCPCONNECTION_H
#define SERVER_TCPCONNECTION_H

#include"EventLoop.h"
#include"HTTPRequest.h"
#include"HTTPResponse.h"

#include<memory>
#include<netinet/in.h>
#include<string>
#include<functional>
#include<vector>

struct Entry;

class TcpConnection : public std::enable_shared_from_this<TcpConnection>
{
public:
    typedef std::shared_ptr<TcpConnection> TcpConnectionPtr; 
    typedef std::function<void(const TcpConnectionPtr&)> ConnectionCallBack;
    typedef std::function<void(const TcpConnectionPtr&)> MessageCallBack;
    typedef std::function<void(const std::string&)> CloseCallBack;

    TcpConnection(const TcpConnection&)=delete;
    void operator=(const TcpConnection&)=delete;

    TcpConnection(EventLoop& loop,int fd,const struct sockaddr_in& peerAddr,const std::string& name);
    ~TcpConnection();

    void setConnectionCallBack(const ConnectionCallBack& cb){
        connectionCallBack_=cb;
    }
    void setMessageCallBack(const MessageCallBack& cb){
        messageCallBack_=cb;
    }
    void setCloseCallBack(const CloseCallBack& cb){
        closeCallBack_=cb;
    }

    void ConnectionClosed();
    void connectionDestroy();

    void send(const std::string&);
    void sendInLoop(const std::string&);

    void closeTimeout();

    void setTCPNoDelay(bool on);

    void inputBufferClear(){
	    inputBuffer_=std::string();
    }

    std::string& getInputBuffer(){
        return inputBuffer_;
    }

    std::string& getOutputBuffer(){
        return outputBuffer_;
    }

    const std::string& name(){
	    return name_;
    }

    HTTPRequest* getHTTPRequest(){
        return &httpRequest_;
    }

    HTTPResponse* getHTTPResponse(){
        return &httpResponse_;
    }

    EventLoop& getLoop(){
        return loop_;
    }

    void connectionEstablished();

    void shutdown();
    void shutdownInLoop();

    
private:
    void readHandle();
    void writeHandle();
    void errorHandle();
    void closeHandle();

    bool EAGAIN_=false;

    /* data */
    EventLoop& loop_;
    int fd_;
    struct sockaddr_in peerAddr_;
    const std::string name_;

    std::shared_ptr<Channel> channelPtr_;

    //std::vector<char> inputBuffer_;
    //std::vector<char> outputBuffer_;

    std::string inputBuffer_;
    std::string outputBuffer_;

    HTTPRequest httpRequest_;
    HTTPResponse httpResponse_;

    ConnectionCallBack connectionCallBack_;
    MessageCallBack messageCallBack_;
    CloseCallBack closeCallBack_;

    bool halfClose_;
    bool disConnected_;

    //static const int BUFFERSIZE;
};



#endif
