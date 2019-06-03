#include"TimingWheel.h"
#include"Channel.h"

#include<sys/timerfd.h>
#include<assert.h>
#include<string.h>
#include<iostream>
#include<unistd.h>

const int TimingWheel::kDestroySeconds=5;

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
    clearConnChannel_->setReadCallBack(std::bind(&TimingWheel::onTimer,this));
    clearConnChannel_->enableReading();
    clearConnChannel_->setET();
}

TimingWheel::~TimingWheel(){
    close(fd_);
}

void TimingWheel::start(){
    struct itimerspec sec;
    bzero(&sec,sizeof sec);
    sec.it_value.tv_sec=1;
    sec.it_interval.tv_sec=1;
timerfd_settime(fd_,NULL,&sec,NULL);
}

void TimingWheel::addConnection(const TcpConnectionPtr& conn){
	//std::cout<<"TimingWheel::addConnection conn use count = "<<conn.use_count()<<std::endl;
    EntryPtr entry(new Entry(conn));
    //std::cout<<conn.use_count()<<std::endl;
    WeakEntryPtr weakEntry(entry);
    //std::cout<<conn.use_count()<<std::endl;
    conn2entry_[conn->name()]=weakEntry;
    //std::cout<<conn.use_count()<<std::endl;
    {
        MutexLockGuard lock(mutex_);
        timingWheel_.back().insert(entry);
    }
    //std::cout<<"TimingWheel::addConnection conn use count read = "<<conn.use_count()<<std::endl;
}

void TimingWheel::onTimer(){
    {
        MutexLockGuard lock(mutex_);
        timingWheel_.push_back(Bucket());
        timingWheel_.pop_front();
    }
    struct itimerspec sec;
    bzero(&sec,sizeof sec);
    sec.it_value.tv_sec=1;
    timerfd_settime(fd_,NULL,&sec,NULL);
    //int one=1;
    //read(fd_,&one,sizeof one);
    std::cout<<"TimingWheel::onTimer() timing wheel go 1s"<<std::endl;
}

void TimingWheel::touchTimer(const std::string& name){
	//std::cout<<"TimingWheel::touchTimer() conn use count = "<<conn.use_count()<<std::endl;
    //WeakEntryPtr weakEntry=std::reinterpret_cast<WeakEntryPtr>(conn->getEntry());
    assert(conn2entry_.find(name)!=conn2entry_.end());
    WeakEntryPtr weakEntry=conn2entry_[name];
    EntryPtr entry=weakEntry.lock();
    if(entry){
        MutexLockGuard lock(mutex_);
        timingWheel_.back().insert(entry);
    }
}
