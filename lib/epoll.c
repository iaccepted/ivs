#include <sys/epoll.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "epoll.h"

#ifdef __cplusplus
extern "C" {
#endif

struct epoll_node {
    int fd;
    void *cb;
    void *arg;
    struct list node;
};

static void destroy_epoll_node(epoll_manager_t *manager, int fd);
static void destroy_all_epoll_nodes(epoll_manager_t *manager);
static struct epoll_node *create_epoll_node(int fd, func_t cb, void *arg);
static void *epoll_loop(void *arg);

epoll_manager_t *create_epoll_manager(uint32_t epoll_size, int epoll_wait_time)
{
    epoll_manager_t *manager = NULL;

    manager = (epoll_manager_t *)calloc(epoll_size, sizeof(epoll_manager_t));
    if (NULL != manager) {
        memset(manager, 0, sizeof(epoll_manager_t));
    }

    manager->epoll_fd = epoll_create(epoll_size);
    if (manager->epoll_fd < 0) {
        printf("create epoll failed, error = %s", strerror(errno));
        free(manager);
        return NULL;
    }
    list_init(&manager->epoll_node_list);
    pthread_mutex_init(&manager->mutex, NULL);
    manager->epoll_wait_time = epoll_wait_time;

    return manager;
}

void destroy_epoll_manager(epoll_manager_t *manager)
{
    if (NULL == manager)
        return;

    destroy_all_epoll_nodes(manager);
    free(manager);
    close(manager->epoll_fd);
    return;
}

int add_epoll_event(epoll_manager_t *manager, int fd, uint32_t _event, func_t cb, void *arg)
{
    int ret = 0;
    struct epoll_node *node = NULL;
    struct epoll_event ev;

    node = create_epoll_node(fd, cb, arg);
    if (NULL == node)
        return -1;

    ev.events = _event;
    ev.data.ptr = node;
    ret = epoll_ctl(manager->epoll_fd, EPOLL_CTL_ADD, fd, &ev);
    if (ret != 0) {
        free(node);
        return ret;
    }

    list_push_back(&manager->epoll_node_list, &node->node);
    return ret;
}

int del_epoll_event(epoll_manager_t *manager, int fd)
{
    int ret = 0;

    ret = epoll_ctl(manager->epoll_fd, EPOLL_CTL_DEL, fd, NULL);
    destroy_epoll_node(manager, fd);
    return ret;
}

int start_epoll_loop(epoll_manager_t *manager)
{
    pthread_create(&manager->epoll_loop_tid, NULL, epoll_loop, (void *)manager);
    pthread_setname_np(manager->epoll_loop_tid, "epoll_loop");
    pthread_join(manager->epoll_loop_tid, NULL);
    return 0;
}

static void *epoll_loop(void *arg)
{
    epoll_manager_t *manager = (epoll_manager_t *)arg;
    struct epoll_event events[MAX_EPOLL_EVENT_NUM];
    int event_num = 0;
    int i = 0;
    struct epoll_node *node = NULL;
    func_t cb = NULL;

    if (NULL == manager)
        return NULL;

    while(1) {
        event_num = epoll_wait(manager->epoll_fd, events, MAX_EPOLL_EVENT_NUM, manager->epoll_wait_time);
        for (i = 0; i < event_num; i++) {
            node = events[i].data.ptr;
            cb = node->cb;
            cb(node->arg);
        }
    }

    return NULL;
}

static struct epoll_node *create_epoll_node(int fd, func_t cb, void *arg)
{
    struct epoll_node *node = NULL;

    node = calloc(1, sizeof(struct epoll_node));
    if (NULL == node) {
        printf("calloc error.");
        return NULL;
    }

    node->arg = arg;
    node->cb = cb;
    node->fd = fd;
    return node;
}

static void destroy_epoll_node(epoll_manager_t *manager, int fd)
{
    struct epoll_node *epoll_node = NULL;
    struct epoll_node *next = NULL;

    LIST_FOR_EACH_SAFE(epoll_node, next, node, &manager->epoll_node_list) {
        if (fd == epoll_node->fd) {
            list_remove(&epoll_node->node);
            free(epoll_node);
            break;
        }
    }
}

static void destroy_all_epoll_nodes(epoll_manager_t *manager)
{
    struct epoll_node *epoll_node = NULL;
    struct epoll_node *next_node = NULL;

    LIST_FOR_EACH_SAFE(epoll_node, next_node, node, &manager->epoll_node_list) {
        list_remove(&epoll_node->node);
        free(epoll_node);
    }
}

#ifdef __cplusplus
}
#endif