#ifndef __ILOG_H__
#define __ILOG_H__ 1

#include <sys/types.h> /* pthread_mutex_t */
#include <stdio.h>

#include "utils/util.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef enum ilog_level {
    ILOG_ERR,
    ILOG_WARN,
    ILOG_INFO,
    ILOG_LEVEL_MAX,
}ilog_level;

struct ilog_info {
    FILE *log_stream;
    enum ilog_level log_level;
    char *log_pattern;
    pthread_mutex_t mutex_lock;
};

#define ILOG(l, fmt, ...) \
    ilog(__FUNCTION__, __LINE__, ILOG_##l, fmt, ##__VA_ARGS__)

int ilog(const char *func, int line, ilog_level level, const char *format, ...) CHECK_FORMAT(4, 5);
int ilog_init(const char *file_name, ilog_level level);
void ilog_uninit();

#ifdef __cplusplus
}
#endif

#endif
