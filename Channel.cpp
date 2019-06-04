#include "Channel.h"

#include <sys/epoll.h>

#include <iostream>

#include "EventLoop.h"

using namespace tinyWS;

const int Channel::kNoneEvent = 0;
const int Channel::kReadEvent = EPOLLIN | EPOLLPRI;
const int Channel::kWriteEvent = EPOLLOUT;

Channel::Channel(EventLoop *loop, int fdArg)
    : loop_(loop),
      fd_(fdArg),
      events_(0),
      revents_(0),
      statusInEpoll_(-1),
      eventHandling_(false) {
}

Channel::~Channel() {
    // 事件处理期间，不会析构
    assert(!eventHandling_);
}

void Channel::handleEvent(Timer::TimeType receiveTime) {
    eventHandling_ = true;
    if ((revents_ & EPOLLHUP) && !(revents_ & EPOLLIN)) {
        if (closeCallback_) {
            closeCallback_();
        }
    }

    if (revents_ & EPOLLIN) {
        std::cout << "Channel::handleEvent() EPOLLIN" << std::endl;
    }

    if (revents_ & EPOLLERR) {
        if (errorCallback_) {
            errorCallback_();
        }
    }

    if (revents_ & (EPOLLIN | EPOLLPRI | EPOLLRDHUP)) {
        if (readCallback_) {
            readCallback_(receiveTime);
        }
    }

    if (revents_ & EPOLLOUT) {
        if (writeCallback_) {
            writeCallback_();
        }
    }
    eventHandling_ = false;
}

void Channel::setReadCallback(const ReadEventCallback &cb) {
    readCallback_ = cb;
}

void Channel::setWriteCallback(const Channel::EventCallback &cb) {
    writeCallback_ = cb;
}
void Channel::setCloseCallback(const Channel::EventCallback &cb) {
    closeCallback_ = cb;
}

void Channel::setErrorCallback(const Channel::EventCallback &cb) {
    errorCallback_ = cb;
}

int Channel::fd() const {
    return fd_;
}

int Channel::events() const {
    return events_;
}

void Channel::setRevents(int revt) {
    revents_ = revt;
}

bool Channel::isNoneEvent() const {
    return revents_ == kNoneEvent;
}

void Channel::enableReading() {
    events_ |= kReadEvent;
    update();
}

void Channel::enableWriting() {
    events_ |= kWriteEvent;
    update();
}

void Channel::disableWriting() {
    events_ &= ~kWriteEvent;
    update();
}

void Channel::disableAll() {
    events_ = kNoneEvent;
    update();
}

bool Channel::isWriting() const {
    return static_cast<bool>(events_ & kWriteEvent);
}

int Channel::statusInEpoll() {
    return statusInEpoll_;
}

void Channel::setStatusInEpoll(int statusInEpoll) {
    statusInEpoll_ = statusInEpoll;
}

EventLoop* Channel::ownerLoop() {
    return loop_;
}

void Channel::remove() {
    loop_->removeChannel(this);
}

void Channel::update() {
    loop_->updateChannel(this);
}

