//
// Created by tong zeng on 2018/6/14.
//
#pragma once

#include <unistd.h>
#include <atomic>
#include <mutex>

struct Process {
    pid_t pid;
    int pipefd[2]{};

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
    static ProcessPool<T> *instance;  // Singleton, 不会用于多线程模型下，不考虑线程安全问题
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
