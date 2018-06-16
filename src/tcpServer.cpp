//
// Created by tong zeng on 2018/6/11.
//
#include "tcpServer.h"
#include "utils.hpp"
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <arpa/inet.h>

int TcpServer::startListen(const char *ip, short port, int backlog) {
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        unixerror("socket fail");
    }
    struct sockaddr_in address{};
    memset(&address, 0, sizeof(address));
    address.sin_family = AF_INET;
    address.sin_port = ntohs(port);
    address.sin_addr.s_addr = inet_addr(ip);

    int ret = bind(sockfd, (struct sockaddr *) (&address), sizeof(address));
    if (ret < 0) {
        unixerror("bind fail");
    }
    ret = listen(sockfd, backlog);
    return ret;
}

int TcpServer::acceptConnect(struct sockaddr_in* client) {
    if (client == nullptr) {
        struct sockaddr_in clientAddress{};
        client = &clientAddress;
    }
    socklen_t clientLength = sizeof(*client);
    return accept(sockfd, (struct sockaddr*) client, &clientLength);
}

int TcpServer::getSockFd() {
    return this->sockfd;
}