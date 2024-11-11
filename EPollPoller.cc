#include "EPollPoller.h"
#include "Logger.h"
#include "Channel.h"

#include <errno.h>
#include <unistd.h>
#include <string.h> 

const int kNew = -1;
const int kAdded = 1;
const int kDeleted = 2;

EPollPoller::EPollPoller(EventLoop* loop) 
    : Poller(loop)
    , epollfd_(::epoll_create1(EPOLL_CLOEXEC))
    , events_(kInitEventListSize) {
    if (epollfd_ < 0) {
        LOG_FATAL("epoll_create error:%d \n", errno)
    }
};
EPollPoller::~EPollPoller() {
    ::close(epollfd_);
}

Timestamp EPollPoller::poll(int timeoutMs, ChannelList* activaeChannels) {
    //日志，最好使用LOG_DEBUG输出降低性能消耗
    LOG_INFO("func = %s => fd total count: %lu\n", __FUNCTION__, channels_.size());

    int numEvents = ::epoll_wait(epollfd_, &*events_.begin(), static_cast<int>(events_.size()), timeoutMs);
    int saveErrno = errno;
    Timestamp now(Timestamp::now()); //now是什么函数？

    if (numEvents > 0) {
        LOG_INFO("%d events happened \n", numEvents);
        fillActiveChannels(numEvents, activaeChannels);
        if (numEvents == events_.size()) {
            events_.resize(events_.size() * 2);
        } 
    } else if (numEvents == 0) {
        LOG_DEBUG("%s timeout! \n", __FUNCTION__);
    } else {
        if (saveErrno != EINTR) {
            errno = saveErrno;
            LOG_ERROR("EPollPoller::poll() err!");
        }
    }
    return now; 
}

void EPollPoller::updateChannel(Channel* channel) {
    const int index = channel->index();
    LOG_INFO("func = %s => fd = %d events = %d index = %d \n",__FUNCTION__, channel->fd(), channel->events(), channel->index());
    /**
     * 具体需要三种修改：注册Poller channels_列表，修改channel->index状态，更新Epoll
     */
    if (index == kNew || index == kDeleted) {
        if (index == kNew) {
            int fd = channel->fd();
            channels_[fd] = channel;
        } //将还未添加过的channel添加到poller channels_列表
        channel->set_index(kAdded);
        update(EPOLL_CTL_ADD, channel);
    } else {
        int fd = channel->fd();
        if (channel->isNoneEvent()) {
            update(EPOLL_CTL_DEL, channel);
            channel->set_index(kDeleted);
        } else {
            update(EPOLL_CTL_MOD, channel);
        }
    }
}

void EPollPoller::removeChannel(Channel* channel) {
    int fd = channel->fd();
    LOG_INFO("func = %s => fd = %d \n",__FUNCTION__, channel->fd());
    channels_.erase(fd);

    int index = channel->index();
    if (index == kAdded) {
        update(EPOLL_CTL_DEL, channel);
    }
    channel->set_index(kNew);
}

void EPollPoller::fillActiveChannels(int numEvents, ChannelList* activeChannels) const {
    for (int i = 0; i < numEvents; ++i) {
        Channel* channel = static_cast<Channel*> (events_[i].data.ptr);
        channel->set_revents(events_[i].events);
        activeChannels->push_back(channel);
    }
}

void EPollPoller::update(int operation, Channel* channel) {
    epoll_event event;
    memset(&event, 0, sizeof event);
    int fd = channel->fd();

    event.events = channel->events();
    event.data.fd = fd;
    event.data.ptr = channel;

    if (::epoll_ctl(epollfd_, operation, fd, &event) < 0) {
        if (operation == EPOLL_CTL_DEL){
            LOG_ERROR("epoll_ctl del error: %d\n", errno);
        } else {
            LOG_FATAL("epoll_ctl add/mod error: %d\n", errno);
        }
    }
}
