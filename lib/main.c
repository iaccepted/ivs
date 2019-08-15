#include <stdio.h>

#include "epoll.h"

static void *
say_hello(void *arg)
{
    printf("hello world.\n");
    return NULL;
}

struct list node_list;
int main()
{
    epoll_manager_t *manager = NULL;
    struct epoll_event ev;

    manager = create_epoll_manager(10, 2);
    //add_epoll_event(manager, STDIN_FILENO, EPOLLIN|EPOLLET, say_hello, NULL);
    start_epoll_loop(manager);
    wait_then_destroy_epoll_manager(manager);

    return 0;
}