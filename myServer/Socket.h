#ifndef SERVER_SOCKET_H
#define SERVER_SOCKET_H

struct sockaddr_in;

class Socket{

public:
    Socket(const Socket&)=delete;
    void operator=(const Socket&)=delete;

    Socket(int port);
    ~Socket();

    void bindAndListening();
    int accept(struct sockaddr_in&);
    void close();

    int fd()const{return socketFd_;}


private:
    bool closed_;

    int socketFd_;
    int port_;

};


#endif
