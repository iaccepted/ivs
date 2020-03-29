#include "netdev_virtio.h"
#include "vhost/vhost_user.h"

static struct netdev *netdev_virtio_create(void)
{
    return NULL;
}

static int netdev_virtio_destroy(struct netdev *netdev)
{
    return 0;
}

static int netdev_virtio_init(struct netdev *netdev)
{
    return 0;
}

static int netdev_virtio_deinit(struct netdev *netdev)
{
    return 0;
}

struct netdev_class virtio_class =
{
    .type = "virtio",
    .create = netdev_virtio_create,
    .destroy = netdev_virtio_destroy,
    .init = netdev_virtio_init,
    .deinit = netdev_virtio_deinit,
};
