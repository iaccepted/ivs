#include "log/ilog.h"

int main()
{
    int i;

    ilog_init("/var/log/ivs.log", ILOG_ERR);

    /* this will no log */
    for(i = 0; i < 100; i++) {
        ilog(__FUNCTION__, __LINE__, ILOG_INFO, "%s", "INFO:hello world from ilog");
    }

    /* log normally */
    for(i = 0; i < 100; i++) {
        ilog(__FUNCTION__, __LINE__, ILOG_ERR, "%s", "hello world from ilog");
    }

    /* log normally */
    for(i = 0; i < 100; i++) {
        ILOG(ERR, "%s-%d", "hello world from ILOG", 222);
    }

    ilog_uninit();
    return 0;
}

