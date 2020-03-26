#ifndef __EPOLL_H__
#define __EPOLL_H__ 1

#include <sys/epoll.h>
#include <unistd.h>
#include <inttypes.h>
#include <pthread.h>

#include "list/list.h"
#include "atom/atom.h"

#define MAX_NAME_LEN 17

//pre declare
struct epoll_node;

typedef void *(*func_t)(void *);
struct epoll_manager{
    int epoll_fd;
    pthread_t epoll_loop_tid;
    char thread_name[MAX_NAME_LEN];
    int epoll_wait_time;
    struct list epoll_node_list;
    pthread_mutex_t mutex;
    atom8_t loop_exit;
};

int epoll_init_manager(uint32_t epoll_size, int epoll_wait_time);

/* the caller should close all fd in epoll_manager */
void epoll_deinit_manager();

/* the caller should close the fd */
int epoll_delete_event(int fd);
int epoll_add_event(int fd, uint32_t _event, func_t cb, void *arg);
int epoll_start_loop(const char *thread_name);
void epoll_stop_loop();

#endif