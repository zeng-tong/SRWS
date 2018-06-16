//
// Created by tong zeng on 2018/6/14.
//

#include <cstdio>
#include <fcntl.h>
#include <sys/epoll.h>
#include <cerrno>
#include <csignal>
#include <cassert>
#include <sys/socket.h>
#include <netinet/in.h>
#include <cstring>
#include <sys/types.h>
#include <sys/wait.h>
#include "processPool.h"

const static int PARENT = 1;
const static int CHILD = 0;
static int sigPipefd[2];

static int setNonBlocking(int fd) {
    int old_option = fcntl(fd, F_GETFL);
    int new_option = old_option | O_NONBLOCK;
    fcntl(fd, F_SETFL, new_option);
    return old_option;
}

static void addFd(int epollFd, int fd) {
    epoll_event event;
    event.data.fd = fd;
    event.events = EPOLLIN | EPOLLET;
    epoll_ctl(epollFd, EPOLL_CTL_ADD, fd, &event);
    setNonBlocking(fd);
}

static void removeFd(int epollFd, int fd) {
    epoll_ctl(epollFd, EPOLL_CTL_DEL, fd, 0);
    close(fd);
}

static void sigHandler(int sig) {
    int saveErrno = errno;
    int msg = sig;
    send(sigPipefd[1], (char *) &msg, 1, 0);
    errno = saveErrno;
}

static void addSig(int sig, void (handler)(int), bool restart = true) {
    struct sigaction sa;
    memset(&sa, '\0', sizeof(sa));
    sa.sa_handler = handler;
    if (restart) {
        sa.sa_flags |= SA_RESTART;
    }
    sigfillset(&sa.sa_mask);
    assert(sigaction(sig, &sa, NULL) != -1);
}

template<typename T>
ProcessPool<T> *ProcessPool<T>::instance = nullptr;
template<typename T>
const int ProcessPool<T>::MAX_PROCESS_NUMBER = 16;// 子进程最大个数
template<typename T>
const int ProcessPool<T>::USER_PER_PROCESS = 65536;//子进程能处理的最大客户数量
template<typename T>
const int ProcessPool<T>::MAX_EVENT_NUMBER = 10000; // epoll 处理的事件数

Process::Process() : pid(-1) {}

template<typename T>
ProcessPool<T>::ProcessPool(int listen_fd, int process_number): listenFd(listen_fd), processNumber(process_number),
                                                                stop(false), idx(-1) {
    assert((processNumber > 0) && (processNumber <= MAX_EVENT_NUMBER));

    subProcess = new Process[processNumber];
    assert(subProcess);

    for (int i = 0; i < processNumber; ++i) {
        int ret = socketpair(PF_UNIX, SOCK_STREAM, 0, subProcess[i].pipefd);
        assert(ret == 0);

        subProcess[i].pid = fork();
        assert(subProcess[i].pid >= 0);

        if (subProcess[i].pid > 0) { // parent
            close(subProcess[i].pipefd[1]);
            continue;
        } else {
            close(subProcess[i].pipefd[0]);
            idx = i;
            break;
        }
    }
}

template<typename T>
ProcessPool<T>::~ProcessPool() {
    delete[] subProcess;
    subProcess = nullptr;
}

template<typename T>
ProcessPool<T> *ProcessPool<T>::getInstance(int listen_fd, int process_number) {
    if (instance == nullptr) {
        instance = new ProcessPool(listen_fd, process_number);
    }
    return instance;
}

template<typename T>
void ProcessPool<T>::setupSigPipe() {
    epollFd = epoll_create(5);
    assert(epollFd != -1);

    int ret = socketpair(PF_UNIX, SOCK_STREAM, 0, sigPipefd);
    setNonBlocking(sigPipefd[1]);
    addFd(epollFd, sigPipefd[0]);
    addSig(SIGCHLD, sigHandler);
    addSig(SIGTERM, sigHandler);
    addSig(SIGINT, sigHandler);
    addSig(SIGPIPE, SIG_IGN);
}

template<typename T>
void ProcessPool<T>::run() {
    if (idx == -1) {
        runParent();
    } else {
        runChild();
    }
}

template<typename T>
void ProcessPool<T>::runChild() {
    setupSigPipe();
    int pipeFd = subProcess[idx].pipefd[1];

    addFd(epollFd, pipeFd);
    epoll_event events[MAX_EVENT_NUMBER];
    T *users = new T[USER_PER_PROCESS];
    assert(users);

    int number = 0;
    int ret = -1;
    while (!stop) {
        number = epoll_wait(epollFd, events, MAX_EVENT_NUMBER, -1);
        if (number < 0 && errno != EINTR) {
            printf("epoll failure\n");
            break;
        }
        for (int i = 0; i < number; ++i) {
            int sockfd = events[i].data.fd;
            if (sockfd == pipeFd && events[i].events & EPOLLIN) {
                int client = 0;
                ret = recv(sockfd, (char *) &client, sizeof(client), 0);
                if ((ret < 0 && errno == EAGAIN) || ret == 0) {
                    continue;
                } else {
                    struct sockaddr_in clientAddress{};
                    socklen_t clientAddrLength = sizeof(clientAddress);
                    int connd = accept(listenFd, (struct sockaddr *) &clientAddress, &clientAddrLength);
                    if (connd < 0) {
                        fprintf(stderr, "%s, %s\n", "accept failure", strerror(errno));
                        continue;
                    }
                    addFd(epollFd, connd);
                    users[connd].init(epollFd, connd, clientAddress);
                }
            } else if (sockfd == sigPipefd[0] && events[i].events & EPOLLIN) {
                int sig;
                char signals[1024];
                ret = recv(sigPipefd[0], signals, sizeof(signals), 0);
                if (ret <= 0) {
                    continue;
                } else {
                    for (int j = 0; j < ret; ++j) {
                        switch (signals[j]) {
                            case SIGCHLD: {
                                pid_t pid;
                                int stat;
                                while ((pid = waitpid(-1, &stat, WNOHANG)) > 0);
                                break;
                            }
                            case SIGTERM:
                            case SIGINT: {
                                stop = true;
                                break;
                            }
                            default:
                                break;
                        }
                    }
                }
            } else if (events[i].events & EPOLLIN) {
                users[sockfd].process();
            } else {
                continue;
            }
        }
    }
    delete[] users;
    users = nullptr;
    close(pipeFd);
    close(epollFd);
}

template<typename T>
void ProcessPool<T>::runParent() {
    setupSigPipe();
    addFd(epollFd, listenFd);
    epoll_event events[MAX_EVENT_NUMBER];

    int subProcessCounter = 0;
    int newConn = 1;
    int number = 0;
    int ret = -1;
    while (!stop) {
        number = epoll_wait(epollFd, events, MAX_EVENT_NUMBER, -1);
        if (number < 0 && errno == EINTR) {
            printf("epoll failure\n");
            break;
        }

        for (int i = 0; i < number; ++i) {
            int sockfd = events[i].data.fd;
            if (sockfd == listenFd) {
                int c = subProcessCounter;
                do {
                    if (subProcess[c].pid != -1) break;
                    c = (c + 1) % processNumber;
                } while (c != processNumber);

                if (subProcess[c].pid == -1) {
                    stop = true;
                    break;
                }
                subProcessCounter = (c + 1) % processNumber;
                send(subProcess[i].pipefd[0], (char *) &newConn, sizeof(newConn), 0);
                printf("send request to child %d\n", c);
            } else if (sockfd == sigPipefd[0] && (events[i].events & EPOLLIN)) {
                int sig;
                char signals[1024];
                ret = recv(sigPipefd[0], signals, sizeof(signals), 0);
                if (ret <= 0) continue;

                for (int i = 0; i < ret; ++i) {
                    switch (signals[i]) {
                        case SIGCHLD: {
                            pid_t pid;
                            int stat;
                            while ((pid = waitpid(-1, &stat, WNOHANG)) > 0) {
                                for (int i = 0; i < processNumber; ++i) {
                                    if (subProcess[i].pid == pid) {
                                        printf("child %d join\n", i);
                                        close(subProcess[i].pipefd[0]);
                                        subProcess[i].pid = -1;
                                    }
                                }
                            }
                            stop = true;
                            for (int i = 0; i < processNumber; ++i) {
                                if (subProcess[i].pid != -1) {
                                    stop = false;
                                    break;
                                }
                            }
                            break;
                        }
                        case SIGTERM:
                        case SIGINT: {
                            printf("kill all the child now\n");
                            for (int i = 0; i < processNumber; ++i) {
                                int pid = subProcess[i].pid;
                                if (pid != -1) kill(pid, SIGTERM);
                            }
                            break;
                        }
                        default:
                            break;
                    }
                }
            } else {
                continue;
            }
        }
    }
    close(epollFd);
}

