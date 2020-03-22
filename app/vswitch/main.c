#include <signal.h>

#include "epoll/epoll.h"
#include "netdev/netdev_virtio.h"
#include "log/ilog.h"

uint16_t app_running = 0;

static void
signal_handler(int signum)
{
    if (signum == SIGINT || signum == SIGTERM) {
        ILOG(INFO, "prepare to exit...");
        signal(signum, SIG_DFL);
        app_running = 0;
    }
}

static void
daemon_exit(void)
{
    stop_epoll_loop();
    ilog_uninit();
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

    app_running = 1;
    while(app_running) {
        sleep(1);
    }

    daemon_exit();
    return 0;
}
