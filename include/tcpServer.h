//
// Created by tong zeng on 2018/6/11.
//

#pragma once

class TcpServer{
public:
    TcpServer();
    ~TcpServer();
    int startListen(const char* ip, short port, int backlog = 5);
    int acceptConnect(struct sockaddr_in* clientAddress = nullptr);
    int getSockFd();
    void disConnect();
    bool isConnected();
private:
    int sockfd;
};
