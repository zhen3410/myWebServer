#include"HTTPResponse.h"
#include"HTTPRequest.h"
#include"TimingWheel.h"

#include<sys/mman.h>
#include<sys/stat.h>
#include<fcntl.h>
#include<map>

std::map<std::string,std::string> HTTPResponse::mime_
    ={{"html","text/html"},
      {"avi","video/x-msvideo"},
      {"bmp","image/bmp"},
      {"c","text/plain"},
      {"doc","application/msword"},
      {"gif","image/gif"},
      {"gz","application/x-gzip"},
      {"htm","text/html"},
      {"ico","image/x-icon"},
      {"jpg","image/jpeg"},
      {"png","image/png"},
      {"txt","text/plain"},
      {"mp3","audio/mp3"}};


HTTPResponse::HTTPResponse(){

}

HTTPResponse::~HTTPResponse(){

}

void HTTPResponse::httpError(const std::string& statusCode){
    std::string bodyBuf;
    bodyBuf+="<html><title>额。。出错了</title>";
    bodyBuf+="<body bgcolor=\"ffffff\">";
    bodyBuf+=statusCode;
    bodyBuf+="<hr><em> ZhangZhen </em>\n</body></html>";

    buf_.clear();
    buf_="HTTP/1.1 "+statusCode+"\r\n";
    buf_+="Content-Type: text/html\r\n";
    buf_+="Connection: Close\r\n";
    buf_+="Content-Length: "+std::to_string(bodyBuf.size())+"\r\n";
    buf_+="Server: ZhangZhen\r\n";
    buf_+="\r\n";
    buf_+=bodyBuf;
}

void HTTPResponse::processRequest(HTTPRequest* req){
    buf_="HTTP/1.1 200 OK\r\n";
    if(req->method()==HTTPRequest::Method::kGet||req->method()==HTTPRequest::Method::kHead){
        std::string filename=req->path();
        if(filename=="/"){
            buf_+="Server: ZhangZhen\r\n";
            buf_+="Content-Type: text/plain\r\n\r\nhello world";
            return;
        }
        buf_+="Server: ZhangZhen\r\n";

        size_t dot_pos=filename.find('.');
        std::string fileType;
        if(dot_pos<0){
            fileType="text/html";
        }else{
            fileType=filename.substr(dot_pos+1);
            if(mime_.find(fileType)==mime_.end()){
                httpError("404 Not Found");
                return;
            }
            fileType=(mime_[fileType]);
        }

        struct stat st;
        if(stat(filename.c_str(),&st)<0){
            httpError("404 Not Found");
            return;
        }
        buf_+="Content-Type: "+fileType+"\r\n";
        buf_+="Content-Length: "+std::to_string(st.st_size)+"\r\n";
        buf_+="\r\n";

        // HEAD 请求不会返回响应体
        if(req->method()==HTTPRequest::Method::kHead)return;

        // todo 读取文件
        int fd=open(filename.c_str(),O_RDONLY,0);
        if(fd<0){
            httpError("404 Not Found");
            return;
        }
        void* mmapRet=mmap(NULL,st.st_size,PROT_READ,MAP_PRIVATE,fd,0);
        if(mmapRet==MAP_FAILED){
            munmap(mmapRet,st.st_size);
            httpError("404 Not Found");
            return;
        }
        const char* fileStart=static_cast<char*>(mmapRet);
        buf_+=std::string(fileStart,fileStart+st.st_size);
        munmap(mmapRet,st.st_size);
        return;
    }
}
