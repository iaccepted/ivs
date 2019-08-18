#ifndef IVS_UTIL_H
#define IVS_UTIL_H

#ifdef __cplusplus
extern "C" {
#endif

#define OBJECT_OFFSETOF(OBJECT, MEMBER) \
    ((char *) &(OBJECT)->MEMBER - (char *) (OBJECT))

#define OBJECT_CONTAINER(OBJECT, POINTER, MEMBER)                  \
    ((typeof(OBJECT)) (void *)                                      \
     ((char *) (POINTER) - OBJECT_OFFSETOF(OBJECT, MEMBER)))

#define INIT_CONTAINER(OBJECT, POINTER, MEMBER) \
    ((OBJECT) = NULL, OBJECT_CONTAINER(OBJECT, POINTER, MEMBER))

#ifdef __cplusplus
}
#endif

#endif
