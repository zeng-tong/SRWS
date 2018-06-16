//
// Created by tong zeng on 2018/6/11.
//
#pragma once

#include <errno.h>
#include <cstdio>
#include <cstring>
#include <strings.h>
#include <cstdlib>
void unixerror(const char* msg) {
    fprintf(stderr, "%s, %s\n", msg, strerror(errno));
    exit(1);
}

class EchoServer {
    //todo: echoServer 测试多进程代码
};
