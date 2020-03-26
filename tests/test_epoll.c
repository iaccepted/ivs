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

    ret = epoll_init_manager(120, 10);
    if (ret) {
        printf("init epoll manager error.");
        return -1;
    }
    epoll_add_event(STDIN_FILENO, EPOLLIN|EPOLLET, say_hello, NULL);
    epoll_start_loop("epoll_loop");

    epoll_stop_loop();
    epoll_deinit_manager();
    return 0;
}

