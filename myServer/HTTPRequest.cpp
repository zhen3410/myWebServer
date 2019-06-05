#include"HTTPRequest.h"

#include<algorithm>

const std::string HTTPRequest::CRLF="\r\n";
const std::string HTTPRequest::CRLFCRLF="\r\n\r\n";

HTTPRequest::HTTPRequest()
{

}

HTTPRequest::~HTTPRequest(){
    
}

bool HTTPRequest::ParseRequest(std::string& msg){
    bool ok=true;
    size_t requestLine=0,requestHeader=0;
    // 当收到完整的消息请求后再进行解析
    if(requestLine=msg.find(CRLF)==std::string::npos)return false;
    if(requestHeader=msg.find(CRLFCRLF)==std::string::npos)return false;
    ok=ParseRequestLine(msg,requestLine);
    if(!ok)return false;
    auto nextLine=msg.find(CRLF,requestHeader);
    auto start=requestHeader;
    while(nextLine!=std::string::npos){
        int pos=start;
        for(;pos<nextLine;pos++){
            if(msg[pos]==':')break;
        }
        headers_[msg.substr(start,pos-start)]=msg.substr(pos+1,nextLine-pos-1);
        start=nextLine+2;
        nextLine=msg.find(CRLF,start);
    }
    body_=msg.substr(start);
    msg.clear();
    return true;
}

bool HTTPRequest::ParseRequestLine(std::string& msg,size_t endPos){
    bool ok=true;
    const char* start=msg.c_str();
    const char* end=start+endPos;
    const char* space=std::find(start,end,' ');
    if(space!=end&&getMethod(start,space)){
        start=space+1;
        space=std::find(start,end,' ');
        if(space!=end){
            const char* question=std::find(start,space,'?');
            if(question!=end){
                path_=std::string(start,question);
                query_=std::string(question+1,space);
            }else{
                path_=std::string(start,space);
            }
            start=space+1;
            std::string v(start,end);
            if(v=="HTTP/1.1"){
                version_=kHTTP11;
            }else if(v=="HTTP/1.0"){
                version_=kHTTP10;
            }else{
                version_=kVInvalid;
                ok=false;
            } 
        }
    }
    return ok;
}
