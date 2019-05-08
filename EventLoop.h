#ifndef SERVER_EVENTLOOP_H
#define SERVER_EVENTLOOP_H

#include<pthread.h>
#include<vector>
#include<memory>

namespace server{

class Poller;
class Channel;

class EventLoop {

public:

    EventLoop(const EventLoop&)=delete;
    void opertor(const EventLoop&)=delete;

    EventLoop();
    ~EventLoop();

    void loop();

    void quit();

    void updateChannel(Channel* channel);
    void removeChannel(Channel* channel);

    void assertInLoopThread(){
        if(!isInLoopThread()){
            abortNotInLoopThread();
        }
    }
    bool isInLoopThread()const{
        return threadId_==pthread_self();
    }

    EventLoop* getEventLoopOfCurrentThread();

private:
    
    void abortNotInLoopThread();

    typedef std::vector<Channel*> ChannelList;

    std::unique_ptr<Poller> poller_;
    ChannelList activeChannels_;

    bool looping_;
    bool quit_;
    const long threadId_;
};

}

#endif
