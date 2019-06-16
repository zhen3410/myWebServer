#ifndef SERVER_HTTPREQUEST_H
#define SERVER_HTTPREQUEST_H

#include<string>
#include<map>
#include<iostream>

class HTTPRequest
{
public:
    enum Method{
        kGet,kPost,kHead,kPut,kDelete,kTrace,kConnect,kOptions,kInvalid
    };
    enum Version{
        kHTTP10,kHTTP11,kVInvalid
    };
    HTTPRequest();
    ~HTTPRequest();

    bool ParseRequest(std::string& msg);
    bool ParseRequestLine(std::string& msg,size_t);

    bool getMethod(const char* start,const char* end){
        std::string msg(start,end);
        std::cout<<"HTTPRequest::getMethod() method = "<<msg<<std::endl;
        if(msg=="GET"){
            method_=kGet;
        }else if(msg=="POST"){
            method_=kPost;
        }else if(msg=="HEAD"){
            method_=kHead;
        }else if(msg=="PUT"){
            method_=kPut;
        }else if(msg=="DELETE"){
            method_=kDelete;
        }else if(msg=="TRACE"){
            method_=kTrace;
        }else if(msg=="CONNECT"){
            method_=kConnect;
        }else if(msg=="OPTIONS"){
            method_=kOptions;
        }else method_=kInvalid;
        return method_!=kInvalid;
    }
    const std::string& path(){
        return path_;
    }
    const std::string& query(){
        return query_;
    }
    Method method(){
        return method_;
    }

private:
    /* data */
    Method method_;
    std::string path_;
    std::string query_;
    Version version_;
    std::map<std::string,std::string> headers_;
    std::string body_;

    const static std::string CRLF;
    const static std::string CRLFCRLF;
};


#endif
