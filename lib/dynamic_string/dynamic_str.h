#ifndef __DYNAMIC_STR_H__
#define __DYNAMIC_STR_H__

#include <stddef.h> /* size_t */
#include <stdarg.h> /* va_list */

#include "utils/util.h"

/* simple dynamic string */
struct sds {
    /* buf has used, not including '\0' */
    size_t len;
    /* all buff can use, not including the extral one byte to put '\0' */
    size_t all;
    char *str;
};

#define SDS_INITIALIZER { 0, 0, NULL }

void sds_init(struct sds *);
void sds_deinit(struct sds *);
void sds_clear(struct sds *);
void sds_put_char(struct sds *, char);
void sds_put_buffer(struct sds *, const char *, size_t n);
void sds_put_str(struct sds *, const char *);
void sds_put_format(struct sds *, const char *, ...) CHECK_FORMAT(2, 3);
void sds_put_va_args(struct sds *sds, const char *format, va_list args);
char *sds_str(struct sds *sds);

#endif /*__DYNAMIC_STR_H__ */
