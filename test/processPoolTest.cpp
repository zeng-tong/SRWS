#include "catch.hpp"
#include "processPool.h"
#include "tcpServer.h"
TEST_CASE("test ParentProcess Connection should return ") {
    TcpServer tcpServer;
    if (tcpServer.startListen("127.0.0.1", 8080) == tcpServer.LISTEN_SUCCESS) {
        ProcessPool* processPool = ProcessPool::getInstance(tcpServer.getSockFd());
        processPool->run();

    }

}
