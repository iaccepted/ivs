#include <signal.h>

#include "vhost/vhost_user.h"
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
    epoll_stop_loop();
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

    ret = epoll_init_manager(120, 10);
    if (ret != 0) {
        ILOG(ERR, "init epoll manager error.");
        goto err;
    }

    server = vhost_user_create_server("tap1");
    if (server == NULL) {
        ILOG(ERR, "create vhost user error.");
        goto err;
    }
    vhost_user_start_server(server);
    epoll_start_loop("epoll_loop");

    app_running = 1;
    while(app_running) {
        sleep(1);
    }

err:
    vhost_user_destroy_server(server);
    daemon_exit();
    return 0;
}
