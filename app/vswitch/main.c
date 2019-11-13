#include "epoll/epoll.h"
#include "netdev/netdev_virtio.h"
#include "log/ilog.h"

int main()
{
    int ret;
    struct netdev_virtio dev;

    ret = init_epoll_manager(120, 10);
    if (ret != 0) {
        ILOG(ERR, "init epoll manager error.");
        return -1;
    }
    dev.vskt.path = "/var/run/tap1";
    dev.vskt.dev = &dev;
    create_vhost_user(&dev.vskt);
    start_vhost_user_server(&dev.vskt);
    start_epoll_loop("epoll_loop");
    stop_epoll_loop();

    return 0;
}