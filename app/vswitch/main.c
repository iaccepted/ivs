#include <signal.h>

#include "vhost/vhost_user.h"
#include "epoll/epoll.h"
#include "netdev/netdev_virtio.h"
#include "log/log.h"

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
    epoll_loop_stop();
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
    struct vhost_user_server *server = NULL;

    ilog_init("/var/log/ivs/ivs.log", ILOG_INFO);

    daemon_register_signal();

    ret = epoll_manager_init(120, 10);
    if (ret != 0) {
        ILOG(ERR, "init epoll manager error.");
        goto err;
    }

    server = vhost_user_server_create("tap1");
    if (server == NULL) {
        ILOG(ERR, "create vhost user error.");
        goto err;
    }
    vhost_user_server_start(server);
    epoll_loop_start("epoll_loop");

    app_running = 1;
    while(app_running) {
        sleep(1);
    }

err:
    vhost_user_server_destroy(server);
    daemon_exit();
    return 0;
}
