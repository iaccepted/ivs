#ifndef __ATOM_H__
#define __ATOM_H__ 1

#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef struct atom8 {
    volatile uint8_t v;
}atom8_t;

#define ATOM_INIT { 0 }

static inline void atom8_init(atom8_t *val)
{
    val->v = 0;
}

static inline uint8_t atom8_get(atom8_t *val)
{
    return val->v;
}

static inline void atom8_set(atom8_t *val, uint8_t v)
{
    val->v = v;
}

static inline void atom8_add(atom8_t *val, uint8_t inc)
{
    __sync_fetch_and_add(&val->v, inc);
}

static inline void atom8_sub(atom8_t *val, uint8_t dec)
{
    __sync_fetch_and_sub(&val->v, dec);
}

#ifdef __cplusplus
}
#endif

#endif