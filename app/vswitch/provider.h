#ifndef IVS_PROVIDER_H
#define IVS_PROVIDER_H 1

#ifdef __cplusplus
extern "C" {
#endif

struct global_db {
};

int init_global_db(void);
int deinit_global_db(void);
struct global_db *get_global_db(void);

#ifdef __cplusplus
}
#endif

#endif
