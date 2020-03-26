#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <sys/prctl.h>

#include "epoll.h"

#define MAX_EPOLL_FD_NUM  (2048)
#define MAX_EPOLL_EVENT_NUM (128)
#define EPOLL_WAIT_TIME_MS   (5)

struct epoll_node {
    int fd;
    void *cb;
    void *arg;
    struct list node;
};

struct epoll_manager manager;

static void epoll_destroy_node(int fd);
static void epoll_destroy_all_nodes();
static struct epoll_node *create_epoll_node(int fd, func_t cb, void *arg);
static void *epoll_loop(void *arg);

int epoll_init_manager(uint32_t epoll_size, int epoll_wait_time)
{
    struct epoll_manager *pmgr = &manager;
    memset(pmgr, 0, sizeof(struct epoll_manager));

    pmgr->epoll_fd = epoll_create(epoll_size);
    if (pmgr->epoll_fd < 0) {
        printf("create epoll failed, error = %s", strerror(errno));
        return -1;
    }
    list_init(&pmgr->epoll_node_list);
    strncpy(pmgr->thread_name, "epoll_loop", sizeof("epoll_loop"));
    pthread_mutex_init(&pmgr->mutex, NULL);
    pmgr->epoll_wait_time = epoll_wait_time;
    atom8_init(&pmgr->loop_exit);

    return 0;
}

void epoll_deinit_manager()
{
    epoll_destroy_all_nodes();
    close(manager.epoll_fd);
    return;
}

int epoll_add_event(int fd, uint32_t _event, func_t cb, void *arg)
{
    int ret = 0;
    struct epoll_node *node = NULL;
    struct epoll_event ev;
    struct epoll_manager *pmgr = &manager;

    node = create_epoll_node(fd, cb, arg);
    if (NULL == node)
        return -1;

    ev.events = _event;
    ev.data.ptr = node;
    ret = epoll_ctl(pmgr->epoll_fd, EPOLL_CTL_ADD, fd, &ev);
    if (ret != 0) {
        free(node);
        return ret;
    }

    list_push_back(&pmgr->epoll_node_list, &node->node);
    return ret;
}

int epoll_delete_event(int fd)
{
    struct epoll_manager *pmgr = &manager;
    int ret = 0;

    ret = epoll_ctl(pmgr->epoll_fd, EPOLL_CTL_DEL, fd, NULL);
    epoll_destroy_node(fd);
    return ret;
}

int epoll_start_loop(const char *thread_name)
{
    struct epoll_manager *pmgr = &manager;
    int ret;

    if (thread_name != NULL) {
        memset(pmgr->thread_name, 0, sizeof(pmgr->thread_name));
        strncpy(pmgr->thread_name, thread_name, MAX_NAME_LEN - 1);
    }

    ret = pthread_create(&pmgr->epoll_loop_tid, NULL, epoll_loop, NULL);
    if (ret != 0) {
        printf("pthread_create error, error = %d\n", ret);
        return -1;
    }

    return 0;
}

void epoll_stop_loop()
{
    struct epoll_manager *pmgr = &manager;

    atom8_set(&pmgr->loop_exit, 1);
    pthread_join(pmgr->epoll_loop_tid, NULL);
}

static void *epoll_loop(void *arg)
{
    struct epoll_manager *pmgr = &manager;
    struct epoll_event events[MAX_EPOLL_EVENT_NUM];
    int event_num = 0;
    int i = 0;
    struct epoll_node *node = NULL;
    func_t cb = NULL;

    (void)prctl(PR_SET_NAME, pmgr->thread_name);
    while(1) {
        event_num = epoll_wait(pmgr->epoll_fd, events, MAX_EPOLL_EVENT_NUM, pmgr->epoll_wait_time);
        for (i = 0; i < event_num; i++) {
            node = events[i].data.ptr;
            cb = node->cb;
            cb(node->arg);
        }
        if (atom8_get(&pmgr->loop_exit) == 1) {
            break;
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

static void epoll_destroy_node(int fd)
{
    struct epoll_node *epoll_node = NULL;
    struct epoll_node *next = NULL;
    struct epoll_manager *pmgr = &manager;

    LIST_FOR_EACH_SAFE(epoll_node, next, node, &pmgr->epoll_node_list) {
        if (fd == epoll_node->fd) {
            list_remove(&epoll_node->node);
            free(epoll_node);
            break;
        }
    }
}

static void epoll_destroy_all_nodes()
{
    struct epoll_manager *pmgr = &manager;
    struct epoll_node *epoll_node = NULL;
    struct epoll_node *next = NULL;

    LIST_FOR_EACH_SAFE(epoll_node, next, node, &pmgr->epoll_node_list) {
        list_remove(&epoll_node->node);
        free(epoll_node);
    }
}
