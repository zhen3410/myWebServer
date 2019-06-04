#include"HTTPResponse.h"
#include"HTTPRequest.h"
#include"TimingWheel.h"


HTTPResponse::HTTPResponse(){

}

HTTPResponse::~HTTPResponse(){

}

void HTTPResponse::httpError(const std::string& statusCode){

}

void HTTPResponse::processRequest(HTTPRequest* req){
    buf_="HTTP/1.1 200 OK\r\n";
    if(req->method()==HTTPRequest::Method::kGet||req->method()==HTTPRequest::Method::kHead){
        if(req->path()=="/"){
            buf_+="Server: ZhangZhen\r\n";
            buf_+="Content Type: text/plain\r\n\r\nhello world";
            return;
        }
        if(req->path()=="/index.html"){
            buf_+="Server: ZhangZhen\r\n";
            buf_+="Content Type: text/html\r\n";
            // todo 读取html文件
        }
    }
}