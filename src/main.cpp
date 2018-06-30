//
// Created by tong on 18-6-17.
//

#include "processPool.hpp"
#include "tcpServer.h"
#include "utils.hpp"

int main(int argc, char* argv[]) {
    const char* ip = "127.0.0.1";
    short port = argc > 1 ? atoi(argv[1]): 8080;
    TcpServer tcpServer;
    tcpServer.startListen(ip, port);
    auto processPool = ProcessPool<EchoServer>::getInstance(tcpServer.getSockFd());
    processPool->run();
    return 0;
}

