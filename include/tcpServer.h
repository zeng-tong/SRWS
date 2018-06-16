//
// Created by tong zeng on 2018/6/11.
//

#pragma once

class TcpServer{
public:
    int startListen(const char* ip, short port, int backlog = 5);
    int acceptConnect(struct sockaddr_in*);
    int getSockFd();
public:
    const int LISTEN_SUCCESS = 0;
private:
    int sockfd;
};
