//
// Created by tong on 18-6-17.
//

#include "processPool.hpp"
#include "tcpServer.h"
#include "utils.hpp"

int main() {
    const char* ip = "127.0.0.1";
    short port = 8080;
    TcpServer tcpServer;
    tcpServer.startListen(ip, port);
    auto processPool = ProcessPool<EchoServer>::getInstance(tcpServer.getSockFd());
    processPool->run();
    return 0;
}

