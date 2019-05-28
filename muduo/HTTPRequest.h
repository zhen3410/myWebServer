#ifndef SERVER_HTTPREQUEST_H
#define SERVER_HTTPREQUEST_H

#include"base/Timestamp.h"

#include<string>

namespace server{


class HTTPRequest
{
public:
    enum Method{
        kInvalid,kGet,kPost,kHead,kPut,kDelete
    };
    enum Version{
        kUnknown,kHttp10,kHttp11
    };
    HTTPRequest():method_(kInvalid),version_(kUnknown){}

    bool setMethod(const char* start,const char* end){
        std::string m(start,end);
        if(m=="GET"){
            method_=kGet;
        }else if(m=="POST"){
            method_=kPost;
        }else if(m=="HEAD"){
            method_=kHead;
        }else if(m=="PUT"){
            method_=kPut;
        }else if(m=="DELETE"){
            method_=kDelete;
        }else{
            method_=kInvalid;
        }
        return method_!=kInvalid;
    }
    void setPath(const char* start,const char* end){
        path_.assign(start,end);
    }
    void setQuery(const char* start,const char* end){
        query_.assign(start,end);
    }
    void setReceiveTime(Timestamp t){
        receiveTime_=t;
    }
    void addHeader(const char* start,const char* colon,const char* end){
        std::string field(start,colon);
        ++colon;
        while(colon<end&&isspace(*colon)){
            ++colon;
        }
        std::string value(colon,end);
        int len=value.size();
        while(!value.empty()&&isspace(value[value.size()-1])){
            --len;
        }
        value.resize(len);
        headers_[field]=value;
    }
    string getHeader(const std::string& field){
        string result;
        auto it=headers_.find(field);
        if(it!=headers_.end()){
            result=it->second;
        }
        return result;
    }

private:
    Method method_;
    Version version_;
    std::string path_;
    std::string query_;
    TimeStamp receiveTime_;
    std::map<std::string,std::string> headers_;
};



#endif