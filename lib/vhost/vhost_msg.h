#ifndef __VHOST_MSG_H__
#define __VHOST_MSG_H__ 1

#include <inttypes.h>
#include <sys/un.h>
#include <linux/limits.h>

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

/* feature */
enum vhost_feature {
    VHOST_F_CSUM = 0, /* device handles packet with partial checksum */
    VHOST_F_MTU = 3, /* device support to report max MTU, driver will use this as max mtu value */
    VHOST_F_MAC = 5, /* device can give mac address */
    VHOST_F_GUEST_TSO4 = 7, /* device can send ipv4 tso pkt to driver */
    VHOST_F_GUEST_TSO6 = 8, /* device can send ipv6 tso pkt to driver */
    VHOST_F_HOST_TSO4 = 11, /* device can handle ipv4 tso from driver */
    VHOST_F_HOST_TSO6 = 11, /* device can handle ipv6 tso from driver */
    VHOST_F_MRG_RXBUF = 15, /* device can receive mergerable buffers */
    VHOST_F_MQ = 22, /* device can support multi-queue */
    VHOST_F_PROTOCOL = 30,
};

/* protocol feature */
enum protocol_feature {
   PROTOCOL_F_MQ = 0,
   PROTOCOL_F_LOG_SHMFD = 1,
   PROTOCOL_F_RARP = 2,
   PROTOCOL_F_REPLY_ACK = 3,
   PROTOCOL_F_SET_MTU = 4,
};

#define VHOST_FEATURES ((1ULL << VHOST_F_MQ) | \
		(1ULL << VHOST_F_MRG_RXBUF) | \
		(1ULL << VHOST_F_PROTOCOL) | \
		(1ULL << VHOST_F_CSUM))

#define VHOST_PROTOCOL_FEATURES ((1ULL << PROTOCOL_F_MQ) | \
		(1ULL << PROTOCOL_F_LOG_SHMFD) | \
		(1ULL << PROTOCOL_F_REPLY_ACK))

/* refer to qemu 4.0 hw/virtio/vhost_user.h */
#define VHOST_MEMORY_MAX_NREGIONS 8
#define VHOST_USER_VERSION 0x1

enum vhost_msg_result {
    /* Message handling failed */
    VHOST_MSG_RESULT_ERR = -1,
    /* Message handling successful */
    VHOST_MSG_RESULT_OK =  0,
    /* Message handling successful and reply prepared */
    VHOST_MSG_RESULT_NEED_REPLY =  1,
    /* Message not handled */
    VHOST_MSG_RESULT_NOT_HANDLED,
};

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
    int fd_num;
} __attribute((packed)) vhost_user_msg;

#define VHOST_USER_HDR_SIZE ((size_t) &(((vhost_user_msg *)0)->payload.u64))

typedef int (*vhost_user_msg_handler_t)(vhost_user_msg *msg);

void *vhost_user_handle_msg(void *arg);

#endif  //__VHOST_MSG_H__
