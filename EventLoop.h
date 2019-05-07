#include<pthread.h>
#include<boost/noncopyable.hpp>

namespace server{

class Poller;

class EventLoop : boost::noncopyable{

public:

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
