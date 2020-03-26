#ifndef __NETDEV_H__
#define __NETDEV_H__ 1

#include <inttypes.h>

#include "list/list.h"

//#include <rte_mbuf.h>

#ifdef __cplusplus
extern "C" {
#endif

#define MAX_PORT_NAME_LEN 64
#define MAX_PKT_BURST 64

struct netdev_class;

struct netdev_info {
};

struct netdev_rxq {
};

struct netdev_txq {
};

struct netdev {
    uint32_t dev_id;
    struct netdev_info *info;
    struct netdev_class *class;
    struct list node;
};

struct packet_batch {
    uint32_t count;
    //struct rte_mbuf *packets[MAX_PKT_BURST];
};

struct netdev_class {
    char *type;
    struct netdev *(*create)();
    int (*destroy)(struct netdev *);
    int (*init)(struct netdev *);
    int (*deinit)(struct netdev *);
    int (*recv)(struct netdev_rxq *, struct packet_batch *);
    int (*send)(struct netdev_txq *, struct packet_batch *);
};

#ifdef __cplusplus
}
#endif

#endif /* __NETDEV_H__ */
