#include"TimingWheel.h"
#include"Channel.h"

#include<sys/timerfd.h>
#include<assert.h>
#include<string.h>

const int TimingWheel::kDestroySeconds=5;

int createTimerFd(){
    int timerfd=timerfd_create(CLOCK_MONOTOMIC,TFD_NONBLOCK|TFD_CLOEXEC);
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
}

TimingWheel::~TimingWheel(){
    close(timerfd);
}

void TimingWheel::start(){
    struct itimerspec sec;
    bzero(&sec,sizeof sec);
    sec.it_value_tv_sec=1;
    sec.it_interval.tv_sec=1;
    timerfd_settime(fd_,NULL,&sec,NULL);
}

void TimingWheel::addConnection(TcpConnectionPtr Conn){
    EntryPtr entry(new Entry(conn));
    WeakEntryPtr weakEntry(entry);
    conn->setEntry(weakEntry);
    {
        MutexLockGuard lock(mutex_);
        TimingWheelQueue.back().insert(entry);
    }
}

void TimingWheel::onTimer(){
    {
        MutexLockGuard lock(mutex_);
        TimingWheelQueue.push_back(Bucket());
        TimingWheelQueue.pop_back();
    }
}

void TimingWheel::touchTimer(TcpConnectionPtr conn){
    WeakEntryPtr weakEntry=conn->getEntry();
    EntryPtr entry=weakEntry.lock();
    if(entry){
        MutexLockGuard lock(mutex_);
        TimingWheelQueue.back().insert(entry);
    }
}