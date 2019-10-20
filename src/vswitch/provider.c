#include <unistd.h>
#include <stdio.h>

#include "epoll/epoll.h"
#include "provider.h"

static struct global_db db;

int init_global_db(void)
{
    int ret;

    ret = init_epoll_manager(120, 5);
    if (ret != 0) {
        printf("init epoll manager error.");
        return -1;
    }
    return 0;
}

int deinit_global_db(void)
{
    deinit_epoll_manager();

    return 0;
}

struct global_db *get_global_db(void)
{
    return &db;
}
