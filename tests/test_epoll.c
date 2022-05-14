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

    ret = epoll_manager_init(120, 10);
    if (ret) {
        printf("init epoll manager error.");
        return -1;
    }
    epoll_event_add(STDIN_FILENO, EPOLLIN|EPOLLET, say_hello, NULL);
    epoll_loop_start("epoll_loop");

    epoll_loop_stop();
    epoll_manager_deinit();
    return 0;
}

