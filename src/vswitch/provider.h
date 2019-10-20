#ifndef IVS_PROVIDER_H
#define IVS_PROVIDER_H 1

struct global_db {
};

int init_global_db(void);
int deinit_global_db(void);
struct global_db *get_global_db(void);

#endif
