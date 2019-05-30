#include"HTTPContext.h"
#include"Buffer.h"

#include<algorithm>

using namespace server;


bool HTTPContext::processRequestLine(const char* begin,const char* end){
    // HTTP消息请求的第一个字段是请求方法，具体格式是
    // 请求方法 | 空格 | URL | 空格 | 协议版本 | 回车符 | 换行符
    bool ok=false;
    const char* start=begin;
    const char* space=std::find(start,end,' ');
    if(space!=end&&request_.setMethod(start,space)){
        start=space+1;
        space=std::find(start,end,' ');
        if(space!=end){
            const char* question=std::find(start,space,'?');
            if(question){
                request_.setPath(start,question);
                request_.setQuery(question+1,space);
            }else{
                request_.setPath(start,space);
            }
            start=space+1;
            ok=end-start==8&&std::equal(start,end,"HTTP/1.");
            if(ok){
                if(*(end-1)=='1'){
                    request_.setVersion(HTTPRequest::kHttp11);
                }else if(*(end-1)=='0'){
                    request_.setVersion(HTTPRequest::kHttp10);
                }else{
                    ok=false;
                }
            }
        }
    }
    return ok;
}

bool HTTPConext::parseRequest(Buffer* buf,Timestamp receiveTime){
    bool ok=true;
    bool hasMore=true;
    while(hasMore){
        if(state_==kExpectRequestLine){
            const char* crlf=buf->findEnter();
            if(crlf){
                ok=processRequestLine(buf->peek(),crlf);
                if(ok){
                    request_.setReceiveTime(receiveTime);
                    buf->retrieveUntil(crlf+2);
                    state_=kExpectHeaders;
                }else{
                    hasMore=false;
                }
            }else{
                hasMore=false;
            }
        }else if(state_==kExpectHeaders){
            const char* crlf=buf->findEnter();
            if(crlf){
                const char* colon=std::find(buf->peek(),crlf,':');
                if(colon!=crlf){
                    request_.addHeader(buf->peek(),colon,crlf);
                }else{
                    state_=kGotAll;
                    hasMore=false;
                }
                buf->retrieveUntil(crlf+2);
            }
        }else if(state_==kExpectBody){
            //TODO
        }
    }
    return ok;
}