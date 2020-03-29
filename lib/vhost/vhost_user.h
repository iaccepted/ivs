#ifndef __VHOST_USER_H__
#define __VHOST_USER_H__ 1

#include <inttypes.h>
#include <sys/un.h>
#include <linux/limits.h>

#include "list/list.h"
#include "netdev/netdev.h"

struct vhost_user_socket;
struct netdev_virtio;

struct vhost_user_server {
    char name[MAX_PORT_NAME_LEN];
    struct netdev_virtio *dev;
    struct vhost_user_socket *vsock;
    int kick_fd;
    int call_fd;
    struct list conn_list;
};

struct vhost_user_server *vhost_user_create_server(char *port_name);
int vhost_user_destroy_server(struct vhost_user_server *server);
int vhost_user_start_server(struct vhost_user_server *server);

#endif /* __VHOST_USER_H__ */
