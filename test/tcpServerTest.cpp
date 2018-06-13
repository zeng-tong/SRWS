//
// Created by tong zeng on 2018/6/11.
//
#include <netinet/in.h>
#include "catch.hpp"
#include "tcpServer.h"

TEST_CASE("tcpServer bind to localhost:8080, success should return  zero") {
    TcpServer tcpServer{};
    REQUIRE(tcpServer.startListen("127.0.0.1", 8080, 5) == 0);
}

TEST_CASE("tcpServer TCP accept, success should return connection descriptor, nonnegative number") {
    TcpServer tcpServer{};
    struct sockaddr_in client{};
    REQUIRE(tcpServer.startListen("127.0.0.1", 8081, 5) == 0);
//    REQUIRE(tcpServer.acceptConnect(&client) > 0); //blocked here if no connection.
}