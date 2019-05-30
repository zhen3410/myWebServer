#ifndef SERVER_BASE_LOGSTREAM_H
#define SERVER_BASE_LOGSTREAM_H

#include<string.h>
#include<string>

namespace server{

const int kSmallBuffer=4096;
const int kLargeBuffer=4096*1024;

template<int Size>
class FixedBuffer
{
public:
    FixedBuffer(const FixedBuffer&)=delete;
    void operator=(const FixedBuffer&)=delete;

    FixedBuffer(/* args */):cur_(data_){}
    ~FixedBuffer();
    void append(const char* buf,size_t len){
        if()
    }
    const char* data()const{return data_;}
    int length()const{
        return static_cast<int>(cur_-data_);
    }
    char* current(){
        return cur;
    }
    int avail()const{
        return static_cast<int>(end()-cur);
    }
    void add(size_t len){
        cur_+=len;
    }
    void reset(){
        cur_=data_;
    }
    void bzero(){
        memset(data_,0,sizeof data_);
    }
    std::string toString()const{
        return std::string(data_,length());
    }

private:
    const char* end()const {return data_+sizeo data_;}
    /* data */
    char data_[Size];
    char* cur_;
};


class LogStream
{
public:
    typedef FixedBuffer<kSmallBuffer> Buffer;
    LogStream(const LogStream&)=delete;
    void operator=(const LogStream&)=delete;

    LogStream(/* args */);
    ~LogStream();

    LogStream& operator<<(bool v){
        buffer_.append(v?"1":"0",1);
        return *this;
    }
    LogStream& operator<<(short);
    LogStream& operator<<(unsigned short);
    LogStream& operator<<(int);
    LogStream& operator<<(unsigned int);
    LogStream& operator<<(long);
    LogStream& operator<<(unsigned long);
    LogStream& operator<<(long long);
    LogStream& operator<<(unsigned long long);

    LogStream& operator<<(const void*);

    LogStream& operator<<(float v){
        *this<<static_cast<double>(v);
        return *this;
    }

    LogStream& operator<<(double);

    LogStream& operator<<(char v){
        buffer_.append(&v,1);
        return *this;
    }

    LogStream& operator<<(const char* str){
        if(str){
            buffer_.append(str,strlen(str));
        }else{
            buffer_.append("(null)",6);
        }
        return *this;
    }
    LogStream& operator<<(const unsigned char* str){
        return operator<<(reinterpret_cast<const char*>(str));
    }
    LogStream& operator<<(const std::string& v){
        buffer_.append(v.c_str(),v.size());
        return *this;
    }
    void append(const char* data,int len){
        buffer_.append(data,len);
    }
    const Buffer& buffer()const{return buffer_;}
    void resetBuffer(){
        buffer_.reset();
    }

private:
    void staticCheck();

    template<typename T>
    void formatInteger(T);

    Buffer buffer_;

    static const int kMaxNumericSize=32;
};


}


#endif;