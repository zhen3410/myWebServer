#ifndef SERVER_HTTPRESPONSE_H
#define SERVER_HTTPRESPONSE_H

#include<string>
#include<map>

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

    static std::map<std::string,std::string> mime_;
};

#endif
