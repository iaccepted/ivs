#ifndef __VHOST_USER_H__
#define __VHOST_USER_H__ 1

#include <inttypes.h>
#include <sys/un.h>
#include <linux/limits.h>

#include "netdev/netdev.h"

typedef struct vhost_user_socket {
    char path[PATH_MAX];
    struct sockaddr_un un;
    int fd;
    int type;
    struct list node;
} vhost_user_socket;

typedef struct vhost_user_server {
    char name[MAX_PORT_NAME_LEN];
    struct netdev_virtio *dev;
    vhost_user_socket *vsock;
    int kick_fd;
    int call_fd;
    struct list conn_list;
} vhost_user_server;

/* refer to qemu 4.0 hw/virtio/vhost_user.h */
#define VHOST_MEMORY_MAX_NREGIONS 8
#define VHOST_USER_VERSION 0x1

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
} vhost_user_request;

typedef struct vhost_user_memory_region {
    uint64_t guest_phys_addr;
    uint64_t memory_size;
    uint64_t userspace_addr;
    uint64_t mmap_offset;
} vhost_user_memory_region;

typedef struct vhost_user_memory {
    uint32_t nregions;
    uint32_t padding;
    vhost_user_memory_region regions[VHOST_MEMORY_MAX_NREGIONS];
} vhost_user_memory;

typedef struct vhost_vring_state {
    unsigned int index;
    unsigned int num;
} vhost_vring_state;

typedef struct vhost_vring_addr {
    unsigned int index;
    /* Option flags:now used to store vhost_user_version */
    unsigned int flags;

    /* Whether log address is valid. If set enables logging. */
#define VHOST_VRING_F_LOG 0

    /* desc ring address */
    uint64_t desc_user_addr;
    /* used ring addr*/
    uint64_t used_user_addr;
    /* avail ring address */
    uint64_t avail_user_addr;
    /* log  */
    uint64_t log_guest_addr;
} vhost_vring_addr;

typedef struct vhost_user_log {
    uint64_t mmap_size;
    uint64_t mmap_offset;
} vhost_user_log;

typedef struct vhost_user_msg {
    vhost_user_request request;

#define VHOST_USER_VERSION_MASK     0x3
#define VHOST_USER_REPLY_MASK       (0x1 << 2)
#define VHOST_USER_NEED_REPLY       (0x1 << 3)
    uint32_t flags;
    uint32_t size; /* the following payload size */
    union {
#define VHOST_USER_VRING_IDX_MASK   0xff
#define VHOST_USER_VRING_NOFD_MASK  (0x1<<8)
        uint64_t u64;
        vhost_vring_state state;
        vhost_vring_addr addr;
        vhost_user_memory memory;
        vhost_user_log log;

        /*TODO:support iotlb for no iommu platform */
        //struct vhost_iotlb_msg iotlb;
    } payload;
    int fds[VHOST_MEMORY_MAX_NREGIONS];
} __attribute((packed)) vhost_user_msg;

#define VHOST_USER_HDR_SIZE ((size_t) &(((vhost_user_msg *)0)->payload.u64))

vhost_user_server *vhost_user_create_server(char *port_name);
int vhost_user_destroy_server(vhost_user_server *server);
int vhost_user_start_server(vhost_user_server *server);

#endif /* __VHOST_USER_H__ */
