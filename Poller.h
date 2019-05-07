#include"EventLoop.h"

#include<boost/noncopyable>

#include<vector>
#include<ctime>

struct pollfd;

namespace server{

class Channel;

class Poller : boost::noncopyable{

public:

	typedef std::vector<Channel*> ChannelList;

	Poller(EventLoop* loop);
	~Poller();

	time_t poll(int timeoutMs,ChannelList* activeChannels);

	void updateChannel(channel* channel);

	void assertInLoopThread(){return ownerloop_->assertInLoopThread();}

private:

	void fillActiveChannels(int numEvents,ChannelList* activeChannels)const;

	typedef std::vector<struct pollfd> PollFdList;
	typedef std::map<int,Channel*> ChannelMap;

	EventLoop* ownerloop_;
	PollFdList pollfds_;
	ChannelMap channels_;

}


}

