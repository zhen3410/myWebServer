#ifndef SERVER_HTTPCONTEXT_H
#define SERVER_HTTPCONTEXT_H

#include"HTTPRequest.h"

namespace server{

class Timestamp;
class Buffer;

class HTTPContext
{
public:
    enum HttpRequestParseState{
        kExpectRequestLine,
        kExpectHeaders,
        kExpectBody,
        kGotAll,
    };

    HTTPContext(/* args */):state_(kExpectRequestLine){}
    
    bool parseRequest(Buffer* buf,Timestamp receiveTime);
    bool processRequestLine(const char* begin,const char* end);
    bool gotAll()const{return state_==kGotAll;}

    void reset(){
        state_=kExpectRequestLine;
        request_=HTTPRequest();
    }
private:

    HttpRequestParseState state_;
    HTTPRequest request_;
};

}

#endif