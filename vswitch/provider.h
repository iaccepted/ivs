#ifndef IVS_PROVIDER_H
#define IVS_PROVIDER_H 1

struct epoll_manager;

struct global_db {
    struct epoll_manager *epoll_manager;
};

int init_global_db(void);
int deinit_global_db(void);
struct global_db *get_global_db(void);

#endif
