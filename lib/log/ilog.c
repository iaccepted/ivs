#include <sys/stat.h>
#include <errno.h>
#include <stdarg.h>
#include <string.h>

#include "log/ilog.h"

struct ilog_info ilog_info = {
    .log_stream = NULL,
    .log_level = ILOG_INFO,
    .log_pattern = NULL,
};

static int ilog_valist(ilog_level level, const char *format, va_list args)
{
    int ret;

    if (ilog_info.log_stream == NULL) {
        ilog_info.log_stream = stdout;
    }

    ret = vfprintf(ilog_info.log_stream, format, args);
    fflush(ilog_info.log_stream);
    return ret;
}

int ilog(ilog_level level, const char *format, ...)
{
    va_list args;

    if (level > ilog_info.log_level) {
        return 0;
    }

    va_start(args, format);
    ilog_valist(level, format, args);
    va_end(args);

    return 0;
}

static int ilog_set_file_stream(const char *file_name)
{
    FILE *f = NULL;

    f = fopen(file_name, "a+");
    if (f == NULL) {
        ilog(ILOG_ERR, "fopen failed, error = %s", strerror(errno));
        return -1;
    }

    if (ilog_info.log_stream != NULL) {
        fclose(ilog_info.log_stream);
    }

    ilog_info.log_stream = f;
    return 0;
}

int ilog_init(const char *file_name, ilog_level log_level)
{
    ilog_set_file_stream(file_name);
    ilog_info.log_level = log_level;

    return 0;
}

void ilog_uninit()
{
    if (ilog_info.log_stream != NULL) {
        fflush(ilog_info.log_stream);
        fclose(ilog_info.log_stream);
        ilog_info.log_stream = NULL;
    }

    ilog_info.log_level = ILOG_INFO;
}
