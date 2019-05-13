#include"EventLoop.h"
#include"Poller.h"
#include"channel.h"

#include<iostream>
#include<assert.h>
#include<sys/eventfd.h>

using namespace server;

namespace{

__thread EventLoop* t_loopInThisThread=0;

const int kPollTimeMs=10000;

int createEventFd(){
    int fd=::eventfd(0,EFD_NONBLOCK|EFD_CLOEXEC);
    if(fd<0){
        std::cerr<<"createEventFd() : failed!"<<std::endl;
        abort();
    }
    return fd;
}

}

EventLoop::EventLoop()
	:looping_(false),
    quit_(false),
    callingPendingFunctors_(false),
	threadId_(pthread_self()),
	poller_(new Poller(this)),
    wakeupFd_(createEventFd()),
    wakeupChannel_(new Channel(this,wakeupFd_))
{
    if(t_loopInThisThread){
        std::cerr<<"another thread exist"<<std::endl;
    }else{
        t_loopInThisThread=this;
    }
    wakeupChannel_->setReadCallback(handleRead);
    wakeupChannel_->enableReading();
}

EventLoop::~EventLoop(){
    assert(!looping_);
    t_loopInThisThread=NULL;
}

void EventLoop::runInLoop(const EventLoop::functor& cb){
    if(isInLoopThread()){
        cb();
    }else{
        queueInLoop(cb);
    }
}

void EventLoop::queueInLoop(const EventLoop::functor& cb){
    {
        MutexGuard lock(mutex_);
        pendingFunctors_.push_back(cb);
    }

    // 由于doPendingFunctors()调用的Functor可能再次调用queueInLoop()，
    // 这时queueInLoop()就必须wakeup()，窦泽新加入的cb就不能被及时调用了
    if(!isInLoopThread()||callingPendingFunctors_){
        wakeup();
    }
}

EventLoop* EventLoop::getEventLoopOfCurrentThread(){
    return t_loopInThisThread;
}

void EventLoop::loop(){
    assert(!looping_);
    assertInLoopThread();
    looping_=true;
    quit_=false;

    while(!quit_){
        activeChannels_.clear();
        poller_->poll(kPollTimeMs,&activeChannels_);
        for(ChannelList::iterator it=activeChannels_.begin();it!=activeChannels_.end();++it){
            (*it)->handleEvent();
        }
        doPendingFunctors();
    }

    //::poll(NULL,0,5*1000);

    std::cout<<"EventLoop"<<threadId_<<"stop looping"<<std::endl;
    looping_=false;
}

void EventLoop::doPendingFunctors(){
    std::vector<Functor> functors;
    callingPendingFunctors_=true;

    {
        MutexGuard lock(mutex_);
        functors.swap(pendingFunctors_);
    }

    for(size_t i=0;i<functors.size();++i){
        functors[i]();
    }

    callingPendingFunctors_=false;
}

void EventLoop::wakeup(){
    uint64_t one=1;
    // sizeof()和sizeof的区别
    // sizeof(int)
    // int a; sizeof a;
    ssize_t n=::write(wakeupFd_,&one,sizeof one);
    if(n!=sizeof one){
        std::cerr<<"EventLoop::wakeup() writes "<<n<<" bytes instead of 8"<<std::endl;
    }
}

void EventLoop::handleRead(){
    uint64_t one=1;
    ssize_t n=::read(wakeupFd_,&one,sizeof one);
    if(n!=sizeof one){
        std::cerr<<"EventLoop::handleRead() reads "<<n<<"bytes instead of 8"<<std::endl;
    }
}

void EventLoop::quit(){
    quit_=true;
    // 可以及时终止循环，否则可能会阻塞在调用队列中
    // 在IO线程中调用quit()会由于Poll超时而退出循环
    if(!isInLoopThread()){
        wakeup();
    }
}

void EventLoop::updateChannel(Channel* channel){
    assert(channel->ownerLoop()==this);
    assertInLoopThread();
    poller_->updateChannel(channel);
}

void EventLoop::removeChannel(Channel* channel){
    assert(channel->ownerLoop()==this);
    assertInLoopThread();
    poller_->removeChannel(channel);
}

void EventLoop::abortNotInLoopThread(){
    std::cerr<<"EventLoop::abortNotInLoopThread() EventLoop was created in threadId_ = " << threadId_ << " , current thread id = " <<pthread_self()<<std::endl;
}
