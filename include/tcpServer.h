//
// Created by tong zeng on 2018/6/11.
//

#pragma once

class TcpServer{
public:
    int startListen(const char*, short, int);
    int acceptConnect(struct sockaddr_in*);
private:
    int sockfd;
};
