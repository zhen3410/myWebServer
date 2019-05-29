#ifndef SERVER_HTTPRESPONSE_H
#define SERVER_HTTPRESPONSE_H

#include<map>
#include<string>

namespace server{

class Buffer;

class HTTPResponse
{

public:
    enum HttpStatusCode{
        kUnknown,
        k200Ok=200,
        k301MovedPermanently=301,
        k400BadRequest=400,
        k404NotFound=404,
    };

    explicit HTTPResponse(bool close):statusCode_(kUnknown),closeConnection_(close){}

    void setStatusCode(HttpStatusCode code){
        statusCode_=code;
    }
    void setStatusMessage(const std::string& message){
        statusMessage_=message;
    }
    void setCloseConnection(bool on){
        closeConnection_=on;
    }
    void setContentType(const std::string& contentType){
        addHeader("Content-Type",contentType);
    }
    void addHeader(const std::string& key,const std::string& value){
        headers_[key]=value;
    }
    void setBody(const std::string& body){
        body_=body;
    }
    void appendToBuffer(Buffer* output)const;
private:
    /* data */
    std::map<std::string,std::string> headers_;
    HttpStatusCode statusCode_;
    std::string statusMessage_;
    bool closeConnection_;
    std::string body_;
};


}


#endif