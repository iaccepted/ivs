#include "log/ilog.h"

int main()
{
    int i;

    ilog_init("/var/log/ivs.log", ILOG_ERR);

    /* this will no log */
    for(i = 0; i < 100; i++) {
        ilog(ILOG_INFO, "%s", "INFO:hello world from ilog\n");
    }

    /* log normally */
    for(i = 0; i < 100; i++) {
        ilog(ILOG_ERR, "%s", "ERR:hello world from ilog\n");
    }

    /* log normally */
    for(i = 0; i < 100; i++) {
        ILOG(ERR, "%s-%d", "ERR:hello world from ILOG", 222);
    }

    ilog_uninit();
    return 0;
}

