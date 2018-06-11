//
// Created by tong zeng on 2018/6/11.
//
#include "catch.hpp"
#include "tcpServer.h"

TEST_CASE("tcpServer bind to localhost:8080, success should return  zero") {
    TcpServer tcpServer;
    REQUIRE(tcpServer.startListen("127.0.0.1", 8080, 5) == 0);
}