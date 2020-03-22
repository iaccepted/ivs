#include <sys/uio.h>
#include <sys/socket.h>

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>

#include "vhost_user.h"
#include "epoll/epoll.h"
#include "log/ilog.h"
#include "netdev/netdev_virtio.h"
#include "memory/memory.h"
#include "dynamic_string/dynamic_str.h"

#define MAX_BACKLOG (256)

/*return bytes# of read on success or negative val on failure.
  read vhost_user_msg header and fds in ancillary data
*/
static int read_fd_and_message_hdr(int sockfd, char *buf, int buflen, int *fds, int fd_num)
{
    struct iovec iov;
    struct msghdr msgh;
    struct cmsghdr *cmsgh = NULL;
    int control_size = fd_num * sizeof(int);
    char control_data[CMSG_SPACE(control_size)];
    int ret;

    memset(control_data, 0, sizeof(control_data));
    memset(&msgh, 0, sizeof(struct msghdr));

    iov.iov_base = buf;
    iov.iov_len = buflen;
    msgh.msg_iov = &iov;
    msgh.msg_iovlen = 1;
    msgh.msg_control = (void *)control_data;
    msgh.msg_controllen = sizeof(control_data);

    ret = recvmsg(sockfd, &msgh, 0);
    if (ret <= 0) {
        /* error log */
        return ret;
    }

     if (msgh.msg_flags & (MSG_TRUNC | MSG_CTRUNC)) {
        /* error log */
        return -1;
    }

    for (cmsgh = CMSG_FIRSTHDR(&msgh); cmsgh != NULL;
         cmsgh = CMSG_NXTHDR(&msgh, cmsgh)) {
         if (cmsgh->cmsg_level == SOL_SOCKET &&
             cmsgh->cmsg_type == SCM_RIGHTS) {
             memcpy(fds, CMSG_DATA(cmsgh), cmsgh->cmsg_len - CMSG_LEN(0));
             break;
         }
    }

    return ret;
}

/* read message from qemu */
static int read_vhost_message(int sockfd, vhost_user_msg *msg)
{
    int ret;

    ret = read_fd_and_message_hdr(sockfd, (char *)msg, VHOST_USER_HDR_SIZE,
        msg->fds, VHOST_MEMORY_MAX_NREGIONS);
    if (ret <= 0)
        return ret;

    if (msg && msg->size) {
        if (msg->size > sizeof(msg->payload)) {
            /* error log */
            return -1;
        }

        /* read the message body */
        ret = read(sockfd, &msg->payload, msg->size);
        if (ret <= 0)
            return ret;
        if (ret != (int)msg->size) {
            /* error log */
            return -1;
        }
    }

    return ret;
}

static int send_fd_and_message(int fd, char *buf, int buf_len, int *fds, int fd_num)
{
    struct iovec iov;
    struct msghdr msgh;
    int control_size = fd_num * sizeof(int);
    char control_data[CMSG_SPACE(control_size)];
    struct cmsghdr *cmsg = NULL;
    int ret;

    memset(&msgh, 0, sizeof(msgh));
    memset(control_data, 0, sizeof(control_data));
    iov.iov_base = buf;
    iov.iov_len = buf_len;
    msgh.msg_iov = &iov;
    msgh.msg_iovlen = 1;

    if (fds && fd_num > 0) {
        msgh.msg_control = control_data;
        msgh.msg_controllen = sizeof(control_data);
        cmsg = CMSG_FIRSTHDR(&msgh);
        if (cmsg == NULL) {
            errno = EINVAL;
            return -1;
        }
        cmsg->cmsg_len = CMSG_LEN(control_size);
        cmsg->cmsg_level = SOL_SOCKET;
        cmsg->cmsg_type = SCM_RIGHTS;
        memcpy(CMSG_DATA(cmsg), fds, control_size);
    }

    do {
        ret = sendmsg(fd, &msgh, 0);
    } while (ret < 0 && errno == EINTR);

    if (ret < 0) {
        return ret;
    }

    return ret;
}

static int send_vhost_message(int sockfd, struct vhost_user_msg *msg)
{
    if (!msg)
        return 0;

    return send_fd_and_message(sockfd, (char *)msg,
        VHOST_USER_HDR_SIZE + msg->size, NULL, 0);
}

static int __attribute__((unused)) send_vhost_reply(int sockfd, struct vhost_user_msg *msg)
{
    if (!msg)
        return 0;

    /* clear the low 3 bit, then set (VHOST_USER_VERSION | VHOST_USER_REPLY_MASK) */
    msg->flags &= ~VHOST_USER_VERSION_MASK;
    msg->flags &= ~VHOST_USER_NEED_REPLY;
    msg->flags |= (VHOST_USER_VERSION | VHOST_USER_REPLY_MASK);

    return send_vhost_message(sockfd, msg);
}

static void *vhost_user_msg_handler(void *arg)
{
    struct vhost_user_msg msg;
    vhost_user_socket *vsock = (vhost_user_socket *)arg;
    int ret;

    ret = read_vhost_message(vsock->fd, &msg);
    if (ret <= 0) {
        if (ret < 0) {
        } else {
        }
        return NULL;
    }

    if (msg.request > VHOST_USER_MAX) {
        ILOG(ERR, "vhost msg is error, request = %d", msg.request);
        return NULL;
    }

    switch(msg.request) {
        case VHOST_USER_GET_FEATURES:
            break;
        case VHOST_USER_SET_FEATURES:
            break;
        case VHOST_USER_SET_OWNER:
            break;
        case VHOST_USER_RESET_OWNER:
            break;
        case VHOST_USER_SET_MEM_TABLE:
            break;
        case VHOST_USER_SET_LOG_BASE:
            break;
        case VHOST_USER_SET_LOG_FD:
            break;
        case VHOST_USER_SET_VRING_NUM:
            break;
        case VHOST_USER_SET_VRING_ADDR:
            break;
        case VHOST_USER_SET_VRING_BASE:
            break;
        case VHOST_USER_SET_VRING_KICK:
            break;
        case VHOST_USER_SET_VRING_CALL:
            break;
        case VHOST_USER_SET_VRING_ERR:
            break;
        case VHOST_USER_GET_PROTOCOL_FEATURES:
            break;
        case VHOST_USER_SET_PROTOCOL_FEATURES:
            break;
        case VHOST_USER_GET_QUEUE_NUM:
            break;
        case VHOST_USER_SET_VRING_ENABLE:
            break;
        default:
            break;
    }

    return NULL;
}

static void *vhost_user_create_conn(void *arg)
{
    vhost_user_socket *vsock = (vhost_user_socket *)arg;
    vhost_user_socket *conn = NULL;
    int fd;
    int ret;

    fd = accept(vsock->fd, NULL, NULL);
    if (fd < 0) {
        return NULL;
    }

    conn = malloc(sizeof(vhost_user_socket));
    conn->fd = fd;
    ret = add_epoll_event(conn->fd, EPOLLIN|EPOLLET, vhost_user_msg_handler, conn);
    if (ret != 0) {
        ILOG(ERR, "epoll operation failed.");
        return NULL;
    }

    return conn;
}

/* unix socket, as server*/
vhost_user_socket *create_vhost_user(char *port_name)
{
    int fd;
    vhost_user_socket *vsock = NULL;
    struct sockaddr_un *un = NULL;
    struct sds sds = SDS_INITIALIZER;

    vsock = (vhost_user_socket *)xzalloc(sizeof(*vsock));
    fd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (fd < 0) {
        xfree(vsock);
        return NULL;
    }
    ILOG(INFO, "vhost user socket created, fd = %d", fd);

    memset(un, 0, sizeof(*un));
    un->sun_family = AF_UNIX;
    sds_put_format(&sds, "/var/run/%s", port_name);
    strncpy(un->sun_path, sds_str(&sds), sizeof(un->sun_path));
    un->sun_path[sizeof(un->sun_path) - 1] = '\0';

    vsock->fd = fd;
    sds_deinit(&sds);
    return vsock;
}

int start_vhost_user_server(vhost_user_socket *vsocket)
{
    int ret;
    int fd = vsocket->fd;
    const char *path = vsocket->path;

    ret = bind(fd, (struct sockaddr *)&vsocket->un, sizeof(vsocket->un));
    if (ret < 0) {
        ILOG(ERR, "failed to bind to %s: %s; remove it and try again",
            path, strerror(errno));
        goto err;
    }
    ILOG(INFO, "bind to %s", path);

    ret = listen(fd, MAX_BACKLOG);
    if (ret < 0) {
        goto err;
    }

    ret = add_epoll_event(fd, EPOLLIN|EPOLLET, vhost_user_create_conn, vsocket);
    if (ret != 0) {
        ILOG(ERR, "epoll operations failed.");
        goto err;
    }
    return 0;

err:
    close(fd);
    return -1;
}
