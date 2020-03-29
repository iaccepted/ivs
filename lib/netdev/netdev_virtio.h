#ifndef __NETDEV_VIRTIO_H__
#define __NETDEV_VIRTIO_H__ 1

#include "netdev.h"

#ifdef __cplusplus
extern "C" {
#endif

struct vhost_user_server;

struct netdev_virtio {
    struct netdev up;
    struct vhost_user_server *server;
};

#ifdef __cplusplus
}
#endif

#endif
