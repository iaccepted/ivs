#ifndef IVS_LIST_H
#define IVS_LIST_H 1

#include <stdbool.h>

#include "utils/util.h"

#ifdef __cplusplus
extern "C" {
#endif

struct list
{
    struct list *prev;
    struct list *next;
};

static inline void list_init(struct list *);
static inline void list_insert(struct list *, struct list *);
static inline struct list * list_remove(struct list *);
static inline void list_push_front(struct list *, struct list *);
static inline void list_push_back(struct list *, struct list *);
static inline bool list_is_empty(const struct list *);

#define LIST_INITIALIZER(LIST) { LIST, LIST }

#define LIST_FOR_EACH(ITER, MEMBER, LIST)                               \
    for (INIT_CONTAINER(ITER, (LIST)->next, MEMBER);                    \
         &(ITER)->MEMBER != (LIST);                                     \
         ASSIGN_CONTAINER(ITER, (ITER)->MEMBER.next, MEMBER))

#define LIST_FOR_EACH_SAFE(ITER, NEXT, MEMBER, LIST) \
    for (INIT_CONTAINER(ITER, (LIST)->next, MEMBER);               \
         (&(ITER)->MEMBER != (LIST)                                \
          ? INIT_CONTAINER(NEXT, (ITER)->MEMBER.next, MEMBER), 1   \
          : 0);                                                    \
         (ITER) = (NEXT))

static inline void list_init(struct list *list)
{
    list->next = list->prev = list;
}

static inline void list_insert(struct list *before, struct list *elem)
{
    elem->prev = before->prev;
    elem->next = before;
    before->prev->next = elem;
    before->prev = elem;
}

static inline struct list *list_remove(struct list *elem)
{
    elem->prev->next = elem->next;
    elem->next->prev = elem->prev;
    return elem->next;
}

static inline void list_push_front(struct list *list, struct list *elem)
{
    list_insert(list->next, elem);
}

static inline void list_push_back(struct list *list, struct list *elem)
{
    list_insert(list, elem);
}

static inline bool list_is_empty(const struct list *list)
{
    return list->next == list;
}

#ifdef __cplusplus
}
#endif

#endif
