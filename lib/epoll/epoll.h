#include <sys/epoll.h>
#include <unistd.h>
#include <inttypes.h>
#include <pthread.h>

#ifndef IVS_EPOLL_H
#define IVS_EPOLL_H 1

#include "list/list.h"

//pre declare
struct epoll_node;

typedef void *(*func_t)(void *);
struct epoll_manager{
    int epoll_fd;
    pthread_t epoll_loop_tid;
    int epoll_wait_time;
    struct list epoll_node_list;
    pthread_mutex_t mutex;
};

int init_epoll_manager(uint32_t epoll_size, int epoll_wait_time);

/* the caller should close all fd in epoll_manager */
void deinit_epoll_manager();

/* the caller should close the fd */
int del_epoll_event(int fd);
int add_epoll_event(int fd, uint32_t _event, func_t cb, void *arg);
int start_epoll_loop(const char *thread_name);

#endif