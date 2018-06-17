//
// Created by tong zeng on 2018/6/16.
//
#pragma once
#include <unistd.h>
#include <sys/socket.h>
class BaseHandler{
public:
    virtual int init(int epollFd, int connd, struct sockaddr_in clientAddress) = 0;
    virtual int process() = 0;
};