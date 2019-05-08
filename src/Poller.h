#ifndef SERVER_POLLER_H
#define SERVER_POLLER_H

#include"EventLoop.h"

#include<vector>
#include<map>
#include<ctime>

struct pollfd;

namespace server{

class Channel;

class Poller {

public:

	Poller(const Poller&)=delete;
	void operator=(const Poller&)=delete;

	typedef std::vector<Channel*> ChannelList;

	Poller(EventLoop* loop);
	~Poller();

	time_t poll(int timeoutMs,ChannelList* activeChannels);

	void updateChannel(Channel* channel);

	void assertInLoopThread(){return ownerloop_->assertInLoopThread();}

private:

	void fillActiveChannels(int numEvents,ChannelList* activeChannels)const;

	typedef std::vector<struct pollfd> PollFdList;
	typedef std::map<int,Channel*> ChannelMap;

	EventLoop* ownerloop_;
	PollFdList pollfds_;
	ChannelMap channels_;

};


}

#endif
