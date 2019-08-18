#include <unistd.h>
#include <stdio.h>

#include "epoll.h"
#include "provider.h"

static struct global_db db = {
    .epoll_manager = NULL,
};

int init_global_db(void)
{
    struct epoll_manager *epoll_mgr = NULL;

    epoll_mgr = create_epoll_manager(10, 2);
    if (epoll_mgr == NULL) {
        printf("create epoll_manager error.");
        return -1;
    }

    db.epoll_manager = epoll_mgr;
    return 0;
}

int deinit_global_db(void)
{
    if (db.epoll_manager != NULL) {
        destroy_epoll_manager(db.epoll_manager);
        db.epoll_manager = NULL;
    }

    return 0;
}

struct global_db *get_global_db(void)
{
    return &db;
}
