#include <sys/uio.h>
#include <sys/socket.h>

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

#include "netdev/netdev.h"
#include "vhost_user.h"
#include "vhost_socket.h"
#include "epoll/epoll.h"
#include "log/log.h"
#include "netdev/netdev_virtio.h"
#include "memory/memory.h"
#include "ds/ds.h"

#define MAX_BACKLOG (256)

struct vhost_user_server *vhost_user_server_create(char *port_name)
{
    struct vhost_user_server *server = NULL;
    struct vhost_user_socket *vsock = NULL;

    server = xzalloc(sizeof(*server));
    list_init(&server->conn_list);
    vsock = vhost_user_socket_create(port_name);
    if (vsock == NULL) {
        ILOG(ERR, "create vhost user socket error.");
        goto err;
    }

    server->vsock = vsock;
    return server;

err:
    xfree(server);
    return NULL;
}

int vhost_user_server_destroy(struct vhost_user_server *server)
{
    struct vhost_user_socket *conn = NULL;
    struct vhost_user_socket *next = NULL;

    if (server == NULL) {
        return 0;
    }

    LIST_FOR_EACH_SAFE(conn, next, node, &server->conn_list) {
        vhost_user_conn_destroy(conn);
    }
    vhost_user_destroy_socket(server->vsock);
    xfree(server);
    return 0;
}

int vhost_user_server_start(struct vhost_user_server *server)
{
    int ret;
    int fd;
    struct vhost_user_socket *vsock = NULL;

    if (server == NULL || server->vsock == NULL) {
        ILOG(ERR, "Invalid parameter.");
        goto err;
    }

    vsock = server->vsock;
    fd = vsock->fd;
    ret = bind(fd, (struct sockaddr *)&vsock->un, sizeof(vsock->un));
    if (ret < 0) {
        ILOG(ERR, "failed to bind to %s: %s; remove it and try again",
            vsock->path, strerror(errno));
        goto err;
    }
    ILOG(INFO, "bind to %s", vsock->path);

    ret = listen(fd, MAX_BACKLOG);
    if (ret < 0) {
        goto err;
    }

    ret = epoll_event_add(fd, EPOLLIN|EPOLLET, vhost_user_conn_create, server);
    if (ret != 0) {
        ILOG(ERR, "epoll operations failed.");
        goto err;
    }
    return 0;

err:
    return -1;
}
