#include <sys/uio.h>
#include <sys/socket.h>

#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <stdio.h>
#include <unistd.h>
#include <inttypes.h>

#include "vhost_msg.h"
#include "vhost_socket.h"
#include "log/log.h"

static int
vhost_user_get_features(vhost_user_msg *msg)
{
    ILOG(INFO, "vhost user get feature.");
    msg->payload.u64 = VHOST_FEATURES;
    msg->size = sizeof(msg->payload.u64);
    msg->fd_num = 0;

    return VHOST_MSG_RESULT_NEED_REPLY;
}

static int
vhost_user_set_features(vhost_user_msg *msg)
{
    uint64_t feature = msg->payload.u64;
    ILOG(INFO, "vhost user set feature,feature = %x.", feature);
    return 0;
}

static int
vhost_user_set_owner(vhost_user_msg *msg)
{
    ILOG(INFO, "vhost user set owner.");
    return VHOST_MSG_RESULT_OK;
}

static int
vhost_user_get_protocol_features(vhost_user_msg *msg)
{
    ILOG(INFO, "vhost user get protocol features.");
    msg->payload.u64 = VHOST_PROTOCOL_FEATURES;
    msg->size = sizeof(msg->payload.u64);
    return VHOST_MSG_RESULT_NEED_REPLY;
}

static int
vhost_user_set_protocol_features(vhost_user_msg *msg)
{
    ILOG(INFO, "vhost user set protocol features, feature = %"PRIu64, msg->payload.u64);
    return VHOST_MSG_RESULT_OK;
}

static int
vhost_user_get_queue_num(vhost_user_msg *msg)
{
    ILOG(INFO, "vhost user get queue num");
    msg->payload.u64 = 1;
    msg->size = sizeof(msg->payload.u64);
    return VHOST_MSG_RESULT_NEED_REPLY;
}

static int
vhost_user_set_vring_call(vhost_user_msg *msg)
{
    uint16_t qid, idx;

    qid = (msg->payload.u64 & 0xff) >> 1;
    idx = (msg->payload.u64 & 0xff) & 1;
    ILOG(INFO, "vhost user set vring call");
    ILOG(INFO, "vhost user set vring call, qid = %u, idx = %u", qid, idx);
    return VHOST_MSG_RESULT_OK;
}

static int
vhost_user_set_mem_table(vhost_user_msg *msg)
{
    ILOG(INFO, "vhost user set mem table");
    return VHOST_MSG_RESULT_OK;
}

static int
vhost_user_reset_owner(vhost_user_msg *msg)
{
    ILOG(INFO, "vhost user reset owner.");
    return VHOST_MSG_RESULT_OK;
}

static int
vhost_user_set_vring_enable(vhost_user_msg *msg)
{
    ILOG(INFO, "vhost user set vring enable.");
    return VHOST_MSG_RESULT_OK;
}
/*return bytes# of read on success or negative val on failure.
  read vhost_user_msg header and fds in ancillary data
*/
static int vhost_fds_and_msghdr_read(int sockfd, char *buf, int buflen, int *fds, int fd_num)
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
static int vhost_msg_read(int sockfd, vhost_user_msg *msg)
{
    int ret;

    ret = vhost_fds_and_msghdr_read(sockfd, (char *)msg, VHOST_USER_HDR_SIZE,
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

static int vhost_send_fd_and_message(int fd, char *buf, int buf_len, int *fds, int fd_num)
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

static int vhost_send_message(int sockfd, struct vhost_user_msg *msg)
{
    if (!msg)
        return 0;

    return vhost_send_fd_and_message(sockfd, (char *)msg,
        VHOST_USER_HDR_SIZE + msg->size, NULL, 0);
}

static int vhost_send_reply(int sockfd, struct vhost_user_msg *msg)
{
    if (!msg)
        return 0;

    /* clear the low 3 bit, then set (VHOST_USER_VERSION | VHOST_USER_REPLY_MASK) */
    msg->flags &= ~VHOST_USER_VERSION_MASK;
    msg->flags &= ~VHOST_USER_NEED_REPLY;
    msg->flags |= (VHOST_USER_VERSION | VHOST_USER_REPLY_MASK);

    return vhost_send_message(sockfd, msg);
}
/*
typedef enum vhost_user_request {
    VHOST_USER_NONE = 0,
    VHOST_USER_GET_FEATURES = 1,
    VHOST_USER_SET_FEATURES = 2,
    VHOST_USER_SET_OWNER = 3,
    VHOST_USER_RESET_OWNER = 4,
    VHOST_USER_SET_MEM_TABLE = 5,
    VHOST_USER_SET_LOG_BASE = 6,
    VHOST_USER_SET_LOG_FD = 7,
    VHOST_USER_SET_VRING_NUM = 8,
    VHOST_USER_SET_VRING_ADDR = 9,
    VHOST_USER_SET_VRING_BASE = 10,
    VHOST_USER_GET_VRING_BASE = 11,
    VHOST_USER_SET_VRING_KICK = 12,
    VHOST_USER_SET_VRING_CALL = 13,
    VHOST_USER_SET_VRING_ERR = 14,
    VHOST_USER_GET_PROTOCOL_FEATURES = 15,
    VHOST_USER_SET_PROTOCOL_FEATURES = 16,
    VHOST_USER_GET_QUEUE_NUM = 17,
    VHOST_USER_SET_VRING_ENABLE = 18,
    VHOST_USER_MAX,
*/

static vhost_user_msg_handler_t vhost_user_msg_handlers[VHOST_USER_MAX] =
{
    [VHOST_USER_GET_FEATURES] = vhost_user_get_features,
    [VHOST_USER_SET_FEATURES] = vhost_user_set_features,
    [VHOST_USER_SET_OWNER] = vhost_user_set_owner,
    [VHOST_USER_RESET_OWNER] = vhost_user_reset_owner,
    [VHOST_USER_SET_MEM_TABLE] = vhost_user_set_mem_table,
    [VHOST_USER_SET_LOG_BASE] = NULL,
    [VHOST_USER_SET_LOG_FD] = NULL,
    [VHOST_USER_SET_VRING_ADDR] = NULL,
    [VHOST_USER_SET_VRING_NUM] = NULL,
    [VHOST_USER_SET_VRING_BASE] = NULL,
    [VHOST_USER_GET_VRING_BASE] = NULL,
    [VHOST_USER_SET_VRING_KICK] = NULL,
    [VHOST_USER_SET_VRING_CALL] = vhost_user_set_vring_call,
    [VHOST_USER_GET_PROTOCOL_FEATURES] = vhost_user_get_protocol_features,
    [VHOST_USER_SET_PROTOCOL_FEATURES] = vhost_user_set_protocol_features,
    [VHOST_USER_GET_QUEUE_NUM] = vhost_user_get_queue_num,
    [VHOST_USER_SET_VRING_ENABLE] = vhost_user_set_vring_enable,
};

void *vhost_user_handle_msg(void *arg)
{
    struct vhost_user_msg msg;
    struct vhost_user_socket *vsock = (struct vhost_user_socket *)arg;
    int ret;

    ret = vhost_msg_read(vsock->fd, &msg);
    if (ret <= 0) {
        ILOG(INFO, "read vhost msg error.");
        return NULL;
    }

    if (msg.request >= VHOST_USER_MAX) {
        ILOG(ERR, "vhost msg is error, request = %d", msg.request);
        return NULL;
    }

    ILOG(INFO, "msg id = %u", msg.request);

    if (vhost_user_msg_handlers[msg.request] == NULL) {
        return NULL;
    }

    ret = vhost_user_msg_handlers[msg.request](&msg);
    switch(ret) {
        case VHOST_MSG_RESULT_ERR:
            break;
        case VHOST_MSG_RESULT_OK:
            break;
        case VHOST_MSG_RESULT_NEED_REPLY:
            vhost_send_reply(vsock->fd, &msg);
            break;
        case VHOST_MSG_RESULT_NOT_HANDLED:
            break;
    }

    return NULL;
}

