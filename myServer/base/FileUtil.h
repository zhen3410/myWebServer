#ifndef SERVER_BASE_FILEUTIL_H
#define SERVER_BASE_FILEUTIL_H

#include<string>
#include<unistd.h>

class AppendFile{

public:
    AppendFile(const AppendFile&)=delete;
    void operator=(const AppendFile&)=delete;

    explicit AppendFile(std::string name);
    ~AppendFile();

    void append(const char* logline,size_t len);
    void flush();
    off_t writtenBytes()const{return writtenBytes_;}

private:

    size_t write(const char* logline,size_t len);

    FILE* fp_;
    char buffer_[64*1024];
    off_t writtenBytes_;

};

#endif
