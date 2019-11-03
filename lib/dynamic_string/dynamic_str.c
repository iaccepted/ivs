#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include <stdlib.h>

#include "dynamic_str.h"
#include "utils/util.h"

#define MIN_STEP (16)

static int sds_expand(struct sds *sds, int need)
{
    char *ptr = NULL;

    if (sds->len + need > sds->all) {
        sds->all += MAX(sds->all, need);
        sds->all = MAX(sds->all, MIN_STEP);
        ptr = realloc(sds->str, sds->all + 1);
        if (ptr == NULL) {
            printf("realloc memory failed");
            return -1;
        }
        sds->str = ptr;
    }
    return 0;
}

void sds_init(struct sds *sds)
{
    sds->str = NULL;
    sds->len = 0;
    sds->all = 0;
}

void sds_deinit(struct sds *sds)
{
    free(sds->str);
    sds->len = 0;
    sds->all = 0;
}

void sds_clear(struct sds *sds)
{
    sds->len = 0;
}

void sds_put_char(struct sds *sds, char ch)
{
    sds_expand(sds, 1);

    sds->str[sds->len] = ch;
    sds->len += 1;
}

void sds_put_buffer(struct sds *sds, const char *buf, size_t n)
{
    sds_expand(sds, n);

    memcpy(sds->str + sds->len, buf, n);
    sds->len += n;
}

void sds_put_str(struct sds *sds, const char *str)
{
    size_t len;

    len = strlen(str);
    sds_expand(sds, len);

    memcpy(sds->str + sds->len, str, len);
    sds->len += len;
}

static void sds_put_va_args(struct sds *sds, const char *format, va_list args)
{
    va_list args_;
    size_t avail_len;
    int need_len;

    avail_len = sds->str ? sds->all - sds->len + 1 : 0;
    va_copy(args_, args);
    /* If the output  was
       truncated  due  to  this  limit  then the return value is the number of
       characters (excluding the terminating null byte) which would have  been
       written  to the final string if enough space had been available.  Thus,
       a return value of size or more means that  the  output  was  truncated.
       print at most avail_len - 1 characters*/
    need_len = vsnprintf(sds->str + sds->len, avail_len, format, args_);
    va_end(args_);

    /* the buff is not enough for format,args + '\0' */
    if (need_len >= avail_len) {
        sds_expand(sds, need_len);
        avail_len = sds->all - sds->len + 1;
        need_len = vsnprintf(sds->str + sds->len, avail_len, format, args);
    }
    sds->len += need_len;
}

void sds_put_format(struct sds *sds, const char *format, ...)
{
    va_list args;

    va_start(args, format);
    sds_put_va_args(sds, format, args);
    va_end(args);
}

char *sds_str(struct sds *sds)
{
    if (sds->str == NULL) {
        sds_expand(sds, MIN_STEP);
    }
    sds->str[sds->len] = '\0';
    return sds->str;
}
