#include <stdio.h>

#include "epoll/epoll.h"

static void *
say_hello(void *arg)
{
    printf("hello world.\n");
    return NULL;
}

int main()
{
    int ret;

    ret = init_epoll_manager(120, 10);
    if (ret) {
        printf("init epoll manager error.");
        return -1;
    }
    add_epoll_event(STDIN_FILENO, EPOLLIN|EPOLLET, say_hello, NULL);
    start_epoll_loop("epoll_loop");

    stop_epoll_loop();
    deinit_epoll_manager();
    return 0;
}

