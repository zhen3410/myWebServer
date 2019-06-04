#ifndef SERVER_HTTPRESPONSE_H
#define SERVER_HTTPRESPONSE_H

#include<string>

class HTTPRequest;

class HTTPResponse
{
public:
    HTTPResponse(/* args */);
    ~HTTPResponse();

    std::string& getResponse(){
        return buf_;
    }

    void httpError(const std::string& statusCode);
    void processRequest(HTTPRequest*);

private:
    /* data */
    std::string buf_;
};

#endif