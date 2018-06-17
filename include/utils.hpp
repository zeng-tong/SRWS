//
// Created by tong zeng on 2018/6/11.
//
#pragma once
#include <errno.h>
#include <cstdio>
#include <cstring>
#include <strings.h>
#include <cstdlib>
#include <string>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <arpa/inet.h>
#include "baseHandler.h"
class Exception{
public:
    static void unixerror(const char *msg) {
        fprintf(stderr, "%s, %s\n", msg, strerror(errno));
        exit(1);
    }
    static void printError(const char* filepath,int line, const char* msg) {
        fprintf(stderr, "%s at line %d : %s, %s\n", filepath, line, msg, strerror(errno));
    }
};
class EchoClient {
public:
    EchoClient() {
        this->sockfd = socket(AF_INET, SOCK_STREAM, 0);
    }
    ~EchoClient() {
        disConnect();
    }
    int startConnect(const char *ip, short port) {
        struct sockaddr_in serverAddress;
        memset(&serverAddress, 0, sizeof(serverAddress));
        serverAddress.sin_port = htons(port);
        serverAddress.sin_family = AF_INET;
        serverAddress.sin_addr.s_addr = inet_addr(ip);
        return connect(this->sockfd, (struct sockaddr *) &serverAddress, sizeof(serverAddress));
    }
    int sendMessage(const char* msg) {
        return write(sockfd, msg, strlen(msg));
    }
    void disConnect() {
        if (this->sockfd > 0) close(sockfd);
    }
    void closeWrite() {
        if (this->sockfd > 0) {
            shutdown(sockfd, SHUT_WR);
        }
    }
    std::string getMessage() {
        char buffer[1024] = {'\0'};
        recv(sockfd, buffer, 1023, 0);
        return std::string(buffer);
    }

private:
    int sockfd;
};
class EchoServer : public BaseHandler {
public:
    int init(int epollFd, int connd, struct sockaddr_in clientAddress) {
        this->connectionFd = connd;
        this->clientAddress = clientAddress;
        return 0;
    }

    int process() {
        char buffer[1024];
        memset(buffer, '\0', 1024);
        int size = recv(connectionFd, buffer, 1024 - 1, 0);
        if (size == 0)
            return -1;
        size = write(connectionFd, buffer, size);
        return size;
    }

private:
    int connectionFd;
    struct sockaddr_in clientAddress;
};