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
    int i = 0;

    manager = create_epoll_manager(10, 2);
    add_epoll_event(manager, STDIN_FILENO, EPOLLIN|EPOLLET, say_hello, NULL);
    start_epoll_loop(manager);
 
    return 0;
}