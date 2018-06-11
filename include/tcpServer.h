//
// Created by tong zeng on 2018/6/11.
//

#ifndef SIMPLE_REACTOR_WEB_SERVER_TCPSERVER_H
#define SIMPLE_REACTOR_WEB_SERVER_TCPSERVER_H
class TcpServer{
public:
    int startListen(const char*, short, int);
private:
    int sockfd;
};
#endif //SIMPLE_REACTOR_WEB_SERVER_TCPSERVER_H
