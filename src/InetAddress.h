#ifndef SERVER_INETADDRESS_H
#define SERVER_INETADDRESS_H

#include<string>
#include<iostream>

namespace server{

class InetAddress{

public:
	explicit InetAddress(uint16_t port){
		memset(&addr_,0,sizeof(addr_));
		addr_.sin_port=htons(port);
		addr_.sin_family=AF_INET;
		addr_.sin_addr.s_addr=htonl(INADDR_ANY);
	}

	InetAddress(string ip,uint16_t port){
		memset(&addr_,0,sizeof(addr_));
		addr_.sin_port=htons(port);
		addr_.sin_family=AF_INET;
		if(inet_ntop(AF_INET,ip.c_str(),&addr_.sin_addr)<=0){
			std::cerr<<"InetAddress():inet_ntop error"<<Std::endl;
		}
	}

	const struct sockadddr* get(){
		return static_cast<const struct sockaddr*>(addr_);
	}

	void setAddr(const struct sockaddr_in& in){addr_=addr;}

	string getInfo(){
		return to_string(addr_.sin_addr.s_addr)+" : "+to_string(addr.sin_port);
	}

private:
	struct sockaddr_in addr_;

}

}


#endif