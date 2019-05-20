#ifndef SERVER_INETADDRESS_H
#define SERVER_INETADDRESS_H

#include<netinet/in.h>
#include<memory.h>
#include<string>
#include<iostream>
#include<sys/socket.h>
#include<arpa/inet.h>

namespace server{


class InetAddress{

public:
	InetAddress(){}
	explicit InetAddress(uint16_t port){
		memset(&addr_,0,sizeof(addr_));
		addr_.sin_port=htons(port);
		addr_.sin_family=AF_INET;
		addr_.sin_addr.s_addr=htonl(INADDR_ANY);
	}
	explicit InetAddress(const struct sockaddr_in& addr)
		:addr_(addr)
	{}

	InetAddress(std::string ip,uint16_t port){
		memset(&addr_,0,sizeof(addr_));
		addr_.sin_port=htons(port);
		addr_.sin_family=AF_INET;
		if(inet_pton(AF_INET,ip.c_str(),&addr_.sin_addr)<=0){
			std::cerr<<"InetAddress():inet_ntop error"<<std::endl;
		}
	}

	//const struct sockadddr* get()const{
	//	//return static_cast<const struct sockaddr*>((const void*)&addr_);
	//	return sockaddr_cast(&addr_);
	//}

	void get(struct sockaddr_in* addr)const{
		addr->sin_port=addr_.sin_port;
		addr->sin_family=addr_.sin_family;
		addr->sin_addr=addr_.sin_addr;
	}

	void setAddr(const struct sockaddr_in& in){addr_=in;}

	std::string getInfo()const{
		return std::to_string(addr_.sin_addr.s_addr)+" : "+std::to_string(ntohs(addr_.sin_port));
	}

private:
	struct sockaddr_in addr_;

};

}


#endif
