#ifndef ZHE_ARYLIST_H
#define ZHE_ARYLIST_H

#include "zhe-package.h"

#define MAKE_ARYLIST_SPEC_type(linkage_, name_, type_, index_type_, max_elems_) \
    typedef struct name_ {                                              \
        index_type_ count;                                              \
        type_ elems[max_elems_];                                        \
    } name_##_t;

#define MAKE_ARYLIST_SPEC_iter_type(linkage_, name_, type_, index_type_, max_elems_) \
    typedef struct name_##_iter {                                       \
        const name_##_t *set;                                           \
        index_type_ cursor;                                             \
    } name_##_iter_t;

#define MAKE_ARYLIST_SPEC_init(linkage_, name_, type_, index_type_, max_elems_) \
    linkage_ void name_##_init(name_##_t *set);

#define MAKE_ARYLIST_SPEC_count(linkage_, name_, type_, index_type_, max_elems_) \
    linkage_ index_type_ name_##_count(const name_##_t *it);

#define MAKE_ARYLIST_SPEC_insert(linkage_, name_, type_, index_type_, max_elems_) \
    linkage_ void name_##_insert(name_##_t *set, type_ elem);

#define MAKE_ARYLIST_SPEC_iter_first(linkage_, name_, type_, index_type_, max_elems_) \
    linkage_ bool name_##_iter_first(name_##_iter_t *it, const name_##_t *set, type_ *elem); \

#define MAKE_ARYLIST_SPEC_iter_next(linkage_, name_, type_, index_type_, max_elems_) \
    linkage_ bool name_##_iter_next(name_##_iter_t *it, type_ *elem);

#define MAKE_ARYLIST_BODY_init(linkage_, name_, type_, index_type_, index_type_sub_, max_elems_) \
    linkage_ void name_##_init(name_##_t *set)                          \
    {                                                                   \
        set->count index_type_sub_ = 0;                                 \
    }

#define MAKE_ARYLIST_BODY_count(linkage_, name_, type_, index_type_, index_type_sub_, max_elems_) \
    linkage_ index_type_ name_##_count(const name_##_t *set)            \
    {                                                                   \
        return set->count;                                              \
    }

#define MAKE_ARYLIST_BODY_insert(linkage_, name_, type_, index_type_, index_type_sub_, max_elems_) \
    linkage_ void name_##_insert(name_##_t *set, type_ elem)            \
    {                                                                   \
        zhe_assert(set->count index_type_sub_ < max_elems_);            \
        set->elems[set->count index_type_sub_] = elem;                  \
        set->count index_type_sub_ = set->count index_type_sub_ + 1;    \
    }

#define MAKE_ARYLIST_BODY_iter_first(linkage_, name_, type_, index_type_, index_type_sub_, max_elems_) \
    linkage_ bool name_##_iter_first(name_##_iter_t *it, const name_##_t *set, type_ *elem) \
    {                                                                   \
        if (set->count index_type_sub_ > 0) {                           \
            *elem = set->elems[0];                                      \
            it->set = set;                                              \
            it->cursor index_type_sub_ = 1;                             \
            return true;                                                \
        } else {                                                        \
            return false;                                               \
        }                                                               \
    }

#define MAKE_ARYLIST_BODY_iter_next(linkage_, name_, type_, index_type_, index_type_sub_, max_elems_) \
    linkage_ bool name_##_iter_next(name_##_iter_t *it, type_ *elem)    \
    {                                                                   \
        if (it->cursor index_type_sub_ < it->set->count index_type_sub_) { \
            *elem = it->set->elems[it->cursor index_type_sub_];         \
            it->cursor index_type_sub_ = it->cursor index_type_sub_ + 1; \
            return true;                                                \
        } else {                                                        \
            return false;                                               \
        }                                                               \
    }

#endif
