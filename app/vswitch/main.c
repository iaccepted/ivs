#include <signal.h>

#include "epoll/epoll.h"
#include "netdev/netdev_virtio.h"
#include "log/ilog.h"

static void
signal_handler(int signum)
{
    if (signum == SIGINT || signum == SIGTERM) {
        ILOG(INFO, "prepare to exit...");
        signal(signum, SIG_DFL);
        kill(getpid(), signum);
    }
}

int main()
{
    int ret;
    struct netdev_virtio dev;

    ilog_init("/var/log/ivs/ivs.log", ILOG_INFO);

    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);

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
    while(1) {
    }
    stop_epoll_loop();

    return 0;
}
