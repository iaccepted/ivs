#ifndef ILOG_H
#define ILOG_H

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
};

#define ILOG(l, fmt, ...) \
    ilog(ILOG_##l, "%s:%u:" fmt "\n", \
        __FUNCTION__, \
        __LINE__, \
        ##__VA_ARGS__)

int ilog(ilog_level level, const char *format, ...) CHECK_FORMAT(2, 3);
int ilog_init(const char *file_name, ilog_level level);
void ilog_uninit();

#ifdef __cplusplus
}
#endif

#endif
