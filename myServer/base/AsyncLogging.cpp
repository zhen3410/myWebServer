#include"AsyncLogging.h"
#include"LogFile.h"
#include"Timestamp.h"

#include<assert.h>

using namespace server;

AsyncLogging::AsyncLogging(const std::string& basename,off_t rollSize,int flushInterval)
    :flushInterval_(flushInterval),
    running_(false),
    basename_(basename),
    rollSize_(rollSize),
    thread_(std::bind(&AsyncLogging::threadFunc,this),"Logging"),
    latch_(1),
    mutex_(),
    cond_(mutex_),
    currentBuffer_(new Buffer),
    nextBuffer_(new Buffer),
    buffers_()
{
    currentBuffer_->bzero();
    nextBuffer_->bzero();
    buffers_.reserve(16);
}

void AsyncLogging::append(const char* logline,int len){
    MutexGuard lock(mutex_);
    if(currentBuffer_->avail()>len){
        currentBuffer_->append(logline,len);
    }else{
        buffers_.push_back(std::move(currentBuffer_));
        if(nextBuffer_){
            currentBuffer_=std::move(nextBuffer_);
        }else{
            currentBuffer_.reset(new Buffer);
        }
        currentBuffer_->append(logline,len);
        cond_.notify();
    }
}

void AsyncLogging::threadFunc(){
    assert(running_==true);
    latch_.countDown();
    LogFile output(basename_,rollSize_,false);
    BufferPtr newBuffer1(new Buffer);
    BufferPtr newBuffer2(new Buffer);
    newBuffer1->bzero();
    newBuffer2->bzero();
    BufferVector bufferToWrite;
    bufferToWrite.reserve(16);
    while(running_){
        {
            MutexGuard lock(mutex_);
            if(buffers_.empty()){
                cond_.waitForSeconds(flushInterval_);
            }
            buffers_.push_back(std::move(currentBuffer_));
            currentBuffer_=std::move(newBuffer1);
            bufferToWrite.swap(buffers_);
            if(!nextBuffer_){
                nextBuffer_=std::move(newBuffer2);
            }
        }
        if(bufferToWrite.size()>25){
            bufferToWrite.erase(bufferToWrite.begin()+2,bufferToWrite.end());
        }
        for(const auto& buffer:bufferToWrite){
            output.append(buffer->data(),buffer->length());
        }
        if(bufferToWrite.size()>2){
            bufferToWrite.resize(2);
        }
        if(!newBuffer1){
            newBuffer1=std::move(bufferToWrite.back());
            bufferToWrite.pop_back();
            newBuffer1->reset();
        }
        if(!newBuffer2){
            newBuffer2=std::move(bufferToWrite.back());
            bufferToWrite.pop_back();
            newBuffer2->reset();
        }
        output.flush();
    }
    output.flush();
}
