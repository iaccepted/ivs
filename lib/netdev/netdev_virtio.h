#ifndef __NETDEV_VIRTIO_H__
#define __NETDEV_VIRTIO_H__ 1

#include "netdev.h"
#include "vhost/vhost_user.h"

#ifdef __cplusplus
extern "C" {
#endif

struct netdev_virtio {
    struct netdev up;
    vhost_user_socket vskt;
};

#ifdef __cplusplus
}
#endif

#endif
