#include"FileUtil.h"

#include<errno.h>
#include<stdio.h>
#include<fcntl.h>
#include<sys/stat.h>
#include<unistd.h>
#include<string.h>

using namespace server;

AppendFile::AppendFile(std::string name)
    :fp_(fopen(name.c_str(),"ae")),
    writtenBytes_(0)
{
    setbuffer(fp_,buffer_,sizeof buffer_);
}

AppendFile::~AppendFile(){
    fclose(fp_);
}

void AppendFile::append(const char* logline,const size_t len){
    size_t n=write(logline,len);
    size_t remain=len-n;
    while(remain>0){
        size_t x=write(logline+n,remain);
        if(x==0){
            int err=ferror(fp_);
            if(err){
                fprintf(stderr,"AppendFile::append() failed %s\n",strerror(err));
            }
            break;
            n+=x;
            remain=len-n;
        }
        writtenBytes_+=len;
    }
}

void AppendFile::flush(){
    fflush(fp_);
}

size_t AppendFile::write(const char* logline,size_t len){
    return fwrite_unlocked(logline,1,len,fp_);
}
