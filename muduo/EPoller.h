#ifndef SERVER_EPOLLER_H
#define SERVER_EPOLLER_H

#include"EventLoop.h"

#include<vector>
#include<map>
#include<ctime>

struct epoll_event;

namespace server{

class Channel;
class Timestamp;

class EPoller {

public:

	EPoller(const EPoller&)=delete;
	void operator=(const EPoller&)=delete;

	typedef std::vector<Channel*> ChannelList;

	EPoller(EventLoop* loop);
	~EPoller();

	Timestamp poll(int timeoutMs,ChannelList* activeChannels);

	void updateChannel(Channel* channel);
	void removeChannel(Channel* channel);

	void assertInLoopThread(){return ownerloop_->assertInLoopThread();}

private:

	void fillActiveChannels(int numEvents,ChannelList* activeChannels);
	void update(int operation,Channel* channel);

	typedef std::vector<struct epoll_event> EPollFdList;
	typedef std::map<int,Channel*> ChannelMap;
	typedef std::map<Channel*,int> FdMap;

	EventLoop* ownerloop_;
	int epollfd_;
	EPollFdList events_;
	ChannelMap channels_;
	FdMap fds_;
};


}

#endif
