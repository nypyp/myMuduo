#pragma once

#include <functional>

#include "Socket.h"
#include "Channel.h"

class InetAddress;
class EventLoop;

class Acceptor {
public:
    using NewConnectionCallback = std::function<void(int sockfd, const InetAddress&)>;

    Acceptor(EventLoop* loop, const InetAddress& listenAddr, bool reuseport);
    ~Acceptor();

    void setNewConnectionCallback(const NewConnectionCallback& cb_) {
        newConnectionCallback_ = std::move(cb_);
    }

    bool listenning() const { return listenning_;}

    void listen();
private:
    void handleRead();

    EventLoop* loop_;
    Socket acceptSocket_;
    Channel acceptChannel_;
    NewConnectionCallback newConnectionCallback_;
    bool listenning_;
};