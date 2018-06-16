//
// Created by tong zeng on 2018/6/14.
//
#pragma once
#include <unistd.h>

struct Process {
    pid_t pid;
    int pipefd[2]{}; // 用于通知子进程调用 accept 建立连接。

    Process();
};

template<typename T>
class ProcessPool {
private:
    static const int MAX_PROCESS_NUMBER;
    static const int USER_PER_PROCESS;
    static const int MAX_EVENT_NUMBER;
    int processNumber;
    int idx;
    int epollFd;
    int listenFd;
    int stop;
    Process *subProcess; // Save information of all sub_process
    static ProcessPool<T> *instance;  // Singleton 
private:
    explicit ProcessPool(int listen_fd, int process_number = 8);
    ~ProcessPool();
    void runChild();
    void runParent();
    void setupSigPipe();
public:
    static ProcessPool<T> *getInstance(int listen_fd, int process_number = 8);
    void run();

};
