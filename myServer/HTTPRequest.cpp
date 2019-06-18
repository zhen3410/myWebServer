#include"HTTPRequest.h"

#include<algorithm>
#include<iostream>

const std::string HTTPRequest::CRLF="\r\n";
const std::string HTTPRequest::CRLFCRLF="\r\n\r\n";

HTTPRequest::HTTPRequest()
{

}

HTTPRequest::~HTTPRequest(){
    
}

bool HTTPRequest::ParseRequest(std::string& msg){
    //std::cout<<"HTTPRequest::ParseRequest() get msg = "<<msg<<std::endl;
    bool ok=true;
    size_t requestLine=0,requestHeader=0;
    // 当收到完整的消息请求后再进行解析
    // == 比 = 优先级高，必须加括号
    if((requestLine=msg.find(CRLF,0))==std::string::npos)return false;
    //std::cout<<"HTTPRequest::ParseRequest() get http request line is "<<msg.substr(0,requestLine)<<" , requestLine = "<<requestLine<<std::endl;
    if((requestHeader=msg.find(CRLFCRLF,0))==std::string::npos)return false;
    //std::cout<<"HTTPRequest::ParseRequest() get http request header is "<<msg.substr(0,requestHeader)<<" , RequestHeader = "<<requestHeader<<std::endl;
    ok=ParseRequestLine(msg,requestLine);
    if(!ok)return false;
    auto nextLine=msg.find(CRLF,requestLine+2);
    auto start=requestLine+2;
    while(nextLine!=std::string::npos){
        //std::cout<<"HTTPRequest::ParseRequest() parse request header is = "<<msg.substr(start,nextLine)<<std::endl;
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
    //std::cout<<"HTTPRequest::ParseRequestLine() parse request line = "<<msg.substr(0,endPos)<<std::endl;
    bool ok=true;
    const char* start=msg.c_str();
    const char* end=start+endPos;
    const char* space=std::find(start,end,' ');
    if(space!=end&&getMethod(start,space)){
        start=space+1;
        //std::cout<<"HTTPRequest::ParseRequestLine() parse rest line is = "<<std::string(start,end)<<std::endl;
        space=std::find(start,end,' ');
        //std::cout<<"HTTPRequest::ParseRequestLine() parse rest line is = "<<std::string(space,end)<<std::endl;
        if(space!=end){
            const char* question=std::find(start,space,'?');
            if(question!=space){
                path_=std::string(start,question);
                query_=std::string(question+1,space);
            }else{
                path_=std::string(start,space);
            }
            //std::cout<<"HTTPRequest::ParseRequestLine() path = "<<path_<<std::endl;
            //std::cout<<"HTTPRequest::ParseRequestLine() query = "<<query_<<std::endl;
            start=space+1;
            std::string v(start,end);
            //std::cout<<"HTTPRequest::ParseRequestLine() version = "<<v<<std::endl;
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
