//
// Created by tong zeng on 2018/6/11.
//

#ifndef SIMPLE_REACTOR_WEB_SERVER_UTILS_H
#define SIMPLE_REACTOR_WEB_SERVER_UTILS_H

#include <errno.h>
#include <cstdio>
#include <cstring>
#include <strings.h>
#include <cstdlib>
void unixerror(const char* msg) {
    fprintf(stderr, "%s, %s\n", msg, strerror(errno));
    exit(1);
}
#endif //SIMPLE_REACTOR_WEB_SERVER_UTILS_H
