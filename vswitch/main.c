#include <stdio.h>

#include "epoll.h"
#include "provider.h"
#include "vhost_user.h"

static void *
say_hello(void *arg)
{
    printf("hello world.\n");
    return NULL;
}

struct list node_list;
int main()
{
    int ret = 0;
    struct global_db *db = NULL;
    int tid = 0;
    struct epoll_manager *manager = NULL;

    ret = init_global_db();
    if (ret != 0) {
        return -1;
    }
    db = get_global_db();
    manager = db->epoll_manager;

    add_epoll_event(manager, STDIN_FILENO, EPOLLIN|EPOLLET, say_hello, NULL);
    start_epoll_loop(manager, "epoll_loop");

    pthread_join(manager->epoll_loop_tid, NULL);
    deinit_global_db();
    return 0;
}