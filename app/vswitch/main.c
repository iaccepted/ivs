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

static void
daemon_register_signal()
{
    signal(SIGINT, signal_handler);
    signal(SIGTERM, signal_handler);
}

int main()
{
    int ret;
    vhost_user_socket *vsock = NULL;

    ilog_init("/var/log/ivs/ivs.log", ILOG_INFO);

    daemon_register_signal();

    ret = init_epoll_manager(120, 10);
    if (ret != 0) {
        ILOG(ERR, "init epoll manager error.");
        goto err;
    }

    vsock = create_vhost_user("tap1");
    if (vsock == NULL) {
        ILOG(ERR, "create vhost user error.");
        goto err;
    }
    start_vhost_user_server(vsock);
    start_epoll_loop("epoll_loop");

    app_running = 1;
    while(app_running) {
        sleep(1);
    }

err:
    close(vsock->fd);
    unlink(vsock->path);
    daemon_exit();
    return 0;
}
