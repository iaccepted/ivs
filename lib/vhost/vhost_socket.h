#ifndef __VHOST_SOCKET_H__
#define __VHOST_SOCKET_H__ 1

#include <inttypes.h>
#include <sys/un.h>
#include <linux/limits.h>

#include "list/list.h"

struct vhost_user_socket {
    char path[PATH_MAX];
    struct sockaddr_un un;
    int fd;
    int type;
    struct list node;
};

void *vhost_user_create_conn(void *);
void vhost_user_destroy_conn(struct vhost_user_socket *);
struct vhost_user_socket *vhost_user_create_socket(char *);
void vhost_user_destroy_socket(struct vhost_user_socket *);

#endif /* __VHOST_SOCKET_H__ */
