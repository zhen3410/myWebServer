#ifndef SERVER_SOCKET_H
#define SERVER_SOCKET_H

namespace server{

class InetAddress;

class Socket{

public:
	Socket(const Socket&)=delete;
	void operator=(const Socket&)=delete;

	explicit Socket(int socket)
		:socketFd_(socket){}
	~Socket();

	int fd()const {return socketFd_;}

	void setReuseAddr(bool);
	void bindAddr(const InetAddress&);

	void listen();
	int accept(InetAddress*);

private:
	const int socketFd_;

	static const int maxListenListNum;

};

} // namespace server


#endif