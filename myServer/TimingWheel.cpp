#include"TimingWheel.h"
#include"Channel.h"

#include<sys/timerfd.h>
#include<assert.h>
#include<string.h>
#include<iostream>
#include<unistd.h>

const int TimingWheel::kDestroySeconds=5*60;
//const std::map<std::string,TimingWheel::WeakEntryPtr> conn2entry_={};

int createTimerFd(){
    int timerfd=timerfd_create(CLOCK_MONOTONIC,TFD_NONBLOCK|TFD_CLOEXEC);
    assert(timerfd>=0);
    return timerfd;
}

TimingWheel::TimingWheel(EventLoop& loop)
    :loop_(loop),
    fd_(createTimerFd()),
    clearConnChannel_(new Channel(loop_,fd_)),
    timingWheel_(kDestroySeconds),
    mutex_()
{
    //std::cout<<"TimingWheel::constructor fd = "<<fd_<<std::endl;
    clearConnChannel_->setReadCallBack(std::bind(&TimingWheel::onTimer,this));
}

TimingWheel::~TimingWheel(){
    close(fd_);
}

void TimingWheel::start(){
    struct itimerspec sec;
    bzero(&sec,sizeof sec);
    sec.it_value.tv_sec=60;
    //sec.it_interval.tv_sec=1;
    timerfd_settime(fd_,NULL,&sec,NULL);    
    clearConnChannel_->enableReading();
    clearConnChannel_->setET();
    clearConnChannel_->addChannel();
}

void TimingWheel::addConnection(const TcpConnectionPtr& conn){
    EntryPtr entry(new Entry(conn));
    WeakEntryPtr weakEntry(entry);
    conn2entry_[conn->name()]=weakEntry;
    {
        MutexLockGuard lock(mutex_);
        timingWheel_.back().insert(entry);
    }
}

void TimingWheel::onTimer(){
    {
        MutexLockGuard lock(mutex_);
        timingWheel_.push_back(Bucket());
        timingWheel_.pop_front();
    }
    struct itimerspec sec;
    bzero(&sec,sizeof sec);
    sec.it_value.tv_sec=60;
    timerfd_settime(fd_,NULL,&sec,NULL);
}

void TimingWheel::touchTimer(const std::string& name){
    assert(conn2entry_.find(name)!=conn2entry_.end());
    WeakEntryPtr weakEntry=conn2entry_[name];
    EntryPtr entry=weakEntry.lock();
    if(entry){
        MutexLockGuard lock(mutex_);
        timingWheel_.back().insert(entry);
    }
}
