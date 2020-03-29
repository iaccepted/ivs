#include <sys/uio.h>
#include <sys/socket.h>

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

#include "vhost_user.h"
#include "vhost_socket.h"
#include "vhost_msg.h"
#include "epoll/epoll.h"
#include "log/ilog.h"
#include "memory/memory.h"
#include "dynamic_string/dynamic_str.h"

#define MAX_BACKLOG (256)

void *vhost_user_create_conn(void *arg)
{
    struct vhost_user_server *server = (struct vhost_user_server *)arg;
    struct vhost_user_socket *conn = NULL;
    struct vhost_user_socket *vsock = NULL;
    int fd;
    int ret;

    if (arg == NULL) {
        return NULL;
    }

    vsock = server->vsock;

    fd = accept(vsock->fd, NULL, NULL);
    if (fd < 0) {
        return NULL;
    }

    conn = xzalloc(sizeof(struct vhost_user_socket));
    conn->fd = fd;
    ret = epoll_add_event(conn->fd, EPOLLIN|EPOLLET, vhost_user_handle_msg, conn);
    if (ret != 0) {
        vhost_user_destroy_conn(conn);
        ILOG(ERR, "epoll operation failed.");
        return NULL;
    }

    list_insert(&server->conn_list, &conn->node);
    return conn;
}

void vhost_user_destroy_conn(struct vhost_user_socket *conn)
{
    if (conn == NULL) {
        return;
    }

    close(conn->fd);
    xfree(conn);
    return;
}

/* unix socket, as server*/
struct vhost_user_socket *vhost_user_create_socket(char *port_name)
{
    int fd;
    struct vhost_user_socket *vsock = NULL;
    struct sockaddr_un *un = NULL;
    struct sds sds = SDS_INITIALIZER;

    vsock = (struct vhost_user_socket *)xzalloc(sizeof(*vsock));
    fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd < 0) {
        xfree(vsock);
        return NULL;
    }
    ILOG(INFO, "vhost user socket created, fd = %d", fd);

    un = &vsock->un;
    memset(un, 0, sizeof(*un));
    un->sun_family = AF_UNIX;
    sds_put_format(&sds, "/var/run/%s", port_name);
    strncpy(un->sun_path, sds_str(&sds), sizeof(un->sun_path));
    un->sun_path[sizeof(un->sun_path) - 1] = '\0';

    vsock->fd = fd;
    strncpy(vsock->path, sds_str(&sds), sizeof(vsock->path));
    vsock->path[sizeof(vsock->path) - 1] = 0;
    sds_deinit(&sds);
    return vsock;
}

void vhost_user_destroy_socket(struct vhost_user_socket *vsock)
{
    if (vsock == NULL) {
        return;
    }

    close(vsock->fd);
    unlink(vsock->path);
    xfree(vsock);
    return;
}
