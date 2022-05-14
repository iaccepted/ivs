#ifndef IVS_UTIL_H
#define IVS_UTIL_H

#ifdef __cplusplus
extern "C" {
#endif

#ifndef typeof
#define typeof __typeof__
#endif

#define OBJECT_OFFSETOF(OBJECT, MEMBER) \
    ((char *) &(OBJECT)->MEMBER - (char *) (OBJECT))

#define ASSIGN_CONTAINER(OBJECT, POINTER, MEMBER) \
    ((OBJECT) = ((typeof(OBJECT)) (void *)        \
    ((char *) (POINTER) - OBJECT_OFFSETOF(OBJECT, MEMBER))))

#define INIT_CONTAINER(OBJECT, POINTER, MEMBER) \
    ((OBJECT) = NULL, ASSIGN_CONTAINER(OBJECT, POINTER, MEMBER))

#define CHECK_FORMAT(a, b) __attribute__((__format__(printf, a, b)))

#define MAX(a, b) ((a) > (b) ? (a) : (b))

#ifdef __cplusplus
}
#endif

#endif
