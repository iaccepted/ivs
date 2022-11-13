#include <unistd.h>
#include <sys/types.h>
#include <errno.h>
#include <stdarg.h>
#include <string.h>
#include <pthread.h>

#include "log/log.h"
#include "ds/ds.h"

static char *ilog_level_names[] = {
    "err",
    "warning",
    "info",
    "extral",
};

struct ilog_info ilog_info = {
    .log_stream = NULL,
    .log_level = ILOG_INFO,
    .log_pattern = NULL,
    .mutex_lock = PTHREAD_MUTEX_INITIALIZER,
};

static char *ilog_level_to_name(ilog_level level)
{
    return ilog_level_names[level];
}

static char *ilog_get_time(char *time_str)
{
    time_t timer=time(NULL);

    strftime(time_str, 20, "%Y-%m-%d %H:%M:%S", localtime(&timer));
    return time_str;
}

static void ilog_format_msg(struct sds *psds, const char *func, int line,
    ilog_level level, const char *format, va_list args)
{
    char stime[32] = { 0 };
    pthread_t pid;

    pid = getpid();

    ilog_get_time(stime);
    sds_put_format(psds, "%s|%s|%lu|%s[%d]: ", stime, ilog_level_to_name(level),
        pid, func, line);
    sds_put_va_args(psds, format, args);
    sds_put_char(psds, '\n');
}

static int ilog_valist(const char *func, int line, ilog_level level, const char *format, va_list args)
{
    int ret;
    struct sds sds = SDS_INITIALIZER;

    if (ilog_info.log_stream == NULL) {
        ilog_info.log_stream = stdout;
    }

    ilog_format_msg(&sds, func, line, level, format, args);
    pthread_mutex_lock(&ilog_info.mutex_lock);
    ret = fprintf(ilog_info.log_stream, sds_str(&sds));
    fflush(ilog_info.log_stream);
    pthread_mutex_unlock(&ilog_info.mutex_lock);

    sds_deinit(&sds);
    return ret;
}

int ilog(const char *func, int line, ilog_level level, const char *format, ...)
{
    int ret;
    va_list args;

    if (level > ilog_info.log_level) {
        return 0;
    }

    va_start(args, format);
    ret = ilog_valist(func, line, level, format, args);
    va_end(args);

    return ret;
}

static int ilog_set_file_stream(const char *file_name)
{
    FILE *f = NULL;

    f = fopen(file_name, "a+");
    if (f == NULL) {
        ILOG(ERR, "fopen failed, error = %s", strerror(errno));
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
