#include <stdio.h>

#include "dynamic_string/dynamic_str.h"

int main()
{
    struct sds sds = SDS_INITIALIZER;
    char buf[] = "abcdefgh";

    sds_put_char(&sds, 'c');
    printf("%s\n", sds_str(&sds));
    sds_clear(&sds);

    sds_put_buffer(&sds, buf, sizeof(buf));
    printf("%s\n", sds_str(&sds));
    sds_clear(&sds);

    sds_put_str(&sds, "hello world");
    printf("%s\n", sds_str(&sds));
    sds_clear(&sds);

    sds_put_format(&sds, "%s-%d-%c-%.2f", "hello", 1, 'c', 0.233);
    printf("%s\n", sds_str(&sds));
    sds_clear(&sds);

    sds_deinit(&sds);

    return 0;
}
