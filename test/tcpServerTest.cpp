//
// Created by tong zeng on 2018/6/11.
//
#include <netinet/in.h>
#include "catch.hpp"
#include "tcpServer.h"

TEST_CASE("tcpServer bind to localhost:8080, success should return  zero") {
    const int LISTEN_SUCCESS = 0;
    TcpServer tcpServer{};
    REQUIRE(tcpServer.startListen("127.0.0.1", 8080) == LISTEN_SUCCESS);
}