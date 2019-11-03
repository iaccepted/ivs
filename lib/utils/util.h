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

/* As explained in the comment above OBJECT_OFFSETOF(), non-GNUC compilers
 * like MSVC will complain about un-initialized variables if OBJECT
 * hasn't already been initialized. To prevent such warnings, INIT_CONTAINER()
 * can be used as a wrapper around ASSIGN_CONTAINER. */
#define INIT_CONTAINER(OBJECT, POINTER, MEMBER) \
    ((OBJECT) = NULL, ASSIGN_CONTAINER(OBJECT, POINTER, MEMBER))

#define CHECK_FORMAT(a, b) __attribute__((__format__(printf, a, b)))

#ifdef __cplusplus
}
#endif

#endif
