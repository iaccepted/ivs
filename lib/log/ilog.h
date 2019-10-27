#ifndef ILOG_H
#define ILOG_H

#include <stdio.h>

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

#define ILOG_FMT(fmt, ...) \
    fmt "",__VA_ARGS__ ""

#define ILOG_FMT_FIRST(fmt, ...) \
    fmt

#define ILOG_FMT_LEFT(fmt, ...) \
    __VA_ARGS__

#define ILOG(l, ...) \
    ilog(ILOG_##l, ILOG_FMT("%s:%s:%u:" ILOG_FMT_FIRST(__VA_ARGS__) "\n", \
        __FILE__, \
        __FUNCTION__, \
        __LINE__, \
        ILOG_FMT_LEFT(__VA_ARGS__,)))

int ilog(ilog_level level, const char *format, ...);
int ilog_init(const char *file_name, ilog_level level);

#ifdef __cplusplus
}
#endif

#endif
