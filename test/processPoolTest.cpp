#include "utils.hpp"
#include "catch.hpp"
#include "processPool.hpp"
#include "tcpServer.h"



TEST_CASE("test echoClient should send data and receive the same") {
    pid_t  pid = fork();
    if (pid == 0) {
        system("python echoServer.py"); //redirect stdout to stdin, serve as echoServer
    } else {
        sleep(1);
        EchoClient echoClient;
        echoClient.startConnect("127.0.0.1", 8080);
        std::string input = "HELLO";
        echoClient.sendMessage(input.c_str());
        auto output = echoClient.getMessage();
        echoClient.disConnect();
        REQUIRE(input == output);
    }
}