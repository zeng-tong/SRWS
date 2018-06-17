#include "utils.hpp"
#include "catch.hpp"
#include "processPool.hpp"
#include "tcpServer.h"


static void sigalrmHandler(int sig) {
    kill(getpid(), SIGINT);
}

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

TEST_CASE("test ProcessPool, echoClient should receive the same data of its sent") {
    const int LISTEN_SUCCESS = 0;
    TcpServer tcpServer;
    EchoClient echoClient;
    const char *ip = "127.0.0.1";
    const short port = 13233;

    if (tcpServer.startListen(ip, port) == LISTEN_SUCCESS) {
        auto pool = ProcessPool<EchoServer>::getInstance(tcpServer.getSockFd(), 8);
        pid_t  pid = fork();
        if (pid > 0) {
            sleep(2); // assure pool run first
            close(tcpServer.getSockFd());
            echoClient.startConnect(ip, port);
            std::string input = "HELLO";
            echoClient.sendMessage(input.c_str());
            auto output = echoClient.getMessage();
            echoClient.disConnect();
            REQUIRE(input == output);
        } else {
            alarm(10);
            signal(SIGALRM, sigalrmHandler);
            pool->run();
        }
        delete pool;
    }
}
