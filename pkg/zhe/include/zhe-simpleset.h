#ifndef ZHE_SIMPLESET_H
#define ZHE_SIMPLESET_H

#include "zhe-package.h"

typedef enum simpleset_insert_result {
    SSIR_EXISTS,
    SSIR_NOSPACE,
    SSIR_SUCCESS
} simpleset_insert_result_t;

#define MAKE_SIMPLESET_SPEC_type(linkage_, name_, key_type_, type_, index_type_, max_elems_) \
    typedef struct name_ {                                              \
        index_type_ count;                                              \
        type_ elems[max_elems_];                                        \
    } name_##_t;

#define MAKE_SIMPLESET_SPEC_iter_type(linkage_, name_, key_type_, type_, index_type_, max_elems_) \
    typedef struct name_##_iter {                                       \
        const name_##_t *set;                                           \
        index_type_ cursor;                                             \
    } name_##_iter_t;

#define MAKE_SIMPLESET_SPEC_init(linkage_, name_, key_type_, type_, index_type_, max_elems_) \
    linkage_ void name_##_init(name_##_t *set);

#define MAKE_SIMPLESET_SPEC_search(linkage_, name_, key_type_, type_, index_type_, max_elems_) \
    linkage_ bool name_##_search(const name_##_t *set, key_type_ key, index_type_ *pos); \

#define MAKE_SIMPLESET_SPEC_contains(linkage_, name_, key_type_, type_, index_type_, max_elems_) \
    linkage_ bool name_##_contains(const name_##_t *set, key_type_ elem); \

#define MAKE_SIMPLESET_SPEC_count(linkage_, name_, key_type_, type_, index_type_, max_elems_) \
    linkage_ index_type_ name_##_count(const name_##_t *it);

#define MAKE_SIMPLESET_SPEC_insert(linkage_, name_, key_type_, type_, index_type_, max_elems_) \
    linkage_ simpleset_insert_result_t name_##_insert(name_##_t *set, type_ elem); \

#define MAKE_SIMPLESET_SPEC_delete(linkage_, name_, key_type_, type_, index_type_, max_elems_) \
    linkage_ bool name_##_delete(name_##_t *set, type_ elem); \

#define MAKE_SIMPLESET_SPEC_iter_first(linkage_, name_, key_type_, type_, index_type_, max_elems_) \
    linkage_ bool name_##_iter_first(name_##_iter_t *it, const name_##_t *set, type_ *elem); \

#define MAKE_SIMPLESET_SPEC_iter_next(linkage_, name_, key_type_, type_, index_type_, max_elems_) \
    linkage_ bool name_##_iter_next(name_##_iter_t *it, type_ *elem);

#define MAKE_SIMPLESET_BODY_search(linkage_, name_, key_type_, type_, index_type_, index_type_sub_, cmp_, key_from_elem_, max_elems_) \
    linkage_ bool name_##_search(const name_##_t *set, key_type_ key, index_type_ *pos) \
    {                                                                   \
        index_type_ i, j;                                               \
        i index_type_sub_ = 0;                                          \
        j = set->count;                                                 \
        while (i index_type_sub_ < j index_type_sub_) {                 \
            index_type_ m;                                              \
            m index_type_sub_ =                                         \
                i index_type_sub_ +                                     \
                (j index_type_sub_ - i index_type_sub_) / 2;            \
            const int c = cmp_(key, key_from_elem_(set->elems[m index_type_sub_])); \
            if (c == 0) {                                               \
                *pos = m;                                               \
                return true;                                            \
            } else if (c < 0) {                                         \
                j = m;                                                  \
            } else {                                                    \
                i index_type_sub_ = m index_type_sub_ + 1;              \
            }                                                           \
        }                                                               \
        *pos = j;                                                       \
        return false;                                                   \
    }

#define MAKE_SIMPLESET_BODY_init(linkage_, name_, key_type_, type_, index_type_, index_type_sub_, cmp_, key_from_elem_, max_elems_) \
    linkage_ void name_##_init(name_##_t *set)                          \
    {                                                                   \
        set->count index_type_sub_ = 0;                                 \
    }

#define MAKE_SIMPLESET_BODY_contains(linkage_, name_, key_type_, type_, index_type_, index_type_sub_, cmp_, key_from_elem_, max_elems_) \
    linkage_ bool name_##_contains(const name_##_t *set, key_type_ elem) \
    {                                                                   \
        index_type_ pos;                                                \
        return name_##_search(set, elem, &pos);                         \
    }

#define MAKE_SIMPLESET_BODY_count(linkage_, name_, key_type_, type_, index_type_, index_type_sub_, cmp_, key_from_elem_, max_elems_) \
    linkage_ index_type_ name_##_count(const name_##_t *set)            \
    {                                                                   \
        return set->count;                                              \
    }

#define MAKE_SIMPLESET_BODY_insert(linkage_, name_, key_type_, type_, index_type_, index_type_sub_, cmp_, key_from_elem_, max_elems_) \
    linkage_ simpleset_insert_result_t name_##_insert(name_##_t *set, type_ elem) \
    {                                                                   \
        index_type_ pos;                                                \
        if (name_##_search(set, key_from_elem_(elem), &pos)) {          \
            return SSIR_EXISTS;                                         \
        } else if (set->count index_type_sub_ == max_elems_) {          \
            return SSIR_NOSPACE;                                        \
        } else {                                                        \
            if (pos index_type_sub_ < set->count index_type_sub_) {     \
                memmove(&set->elems[pos index_type_sub_+1],             \
                        &set->elems[pos index_type_sub_],               \
                        (set->count index_type_sub_ - pos index_type_sub_) * sizeof(set->elems[0])); \
            }                                                           \
            set->elems[pos index_type_sub_] = elem;                     \
            set->count index_type_sub_ = set->count index_type_sub_ + 1; \
            return SSIR_SUCCESS;                                        \
        }                                                               \
    }

#define MAKE_SIMPLESET_BODY_delete(linkage_, name_, key_type_, type_, index_type_, index_type_sub_, cmp_, key_from_elem_, max_elems_) \
    linkage_ bool name_##_delete(name_##_t *set, type_ elem) \
    {                                                                   \
        index_type_ pos;                                                \
        if (!name_##_search(set, key_from_elem_(elem), &pos)) {         \
            return false;                                               \
        } else {                                                        \
            if (pos index_type_sub_ + 1 < set->count index_type_sub_) { \
                memmove(&set->elems[pos index_type_sub_],               \
                        &set->elems[pos index_type_sub_+1],             \
                        (set->count index_type_sub_ - pos index_type_sub_ - 1) * sizeof(set->elems[0])); \
            }                                                           \
            set->count index_type_sub_ = set->count index_type_sub_ - 1; \
            return true;                                                \
        }                                                               \
    }

#define MAKE_SIMPLESET_BODY_iter_first(linkage_, name_, key_type_, type_, index_type_, index_type_sub_, cmp_, key_from_elem_, max_elems_) \
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

#define MAKE_SIMPLESET_BODY_iter_next(linkage_, name_, key_type_, type_, index_type_, index_type_sub_, cmp_, key_from_elem_, max_elems_) \
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

#define MAKE_SIMPLESET_ALIAS_SPEC_type(linkage_, name_, base_name_, key_type_, type_, index_type_, base_type_, base_index_type_) \
    typedef struct name_ {                                  \
        base_name_##_t x_##base_name_;                      \
    } name_##_t;

#define MAKE_SIMPLESET_ALIAS_SPEC_iter_type(linkage_, name_, base_name_, key_type_, type_, index_type_, base_type_, base_index_type_) \
    typedef struct name_##iter {                            \
        base_name_##_iter_t x_##base_name_;                 \
    } name_##_iter_t;

#define MAKE_SIMPLESET_ALIAS_SPEC_init(linkage_, name_, base_name_, key_type_, type_, index_type_, base_type_, base_index_type_) \
    linkage_ void name_##_init(name_##_t *set);

#define MAKE_SIMPLESET_ALIAS_SPEC_search(linkage_, name_, base_name_, key_type_, type_, index_type_, base_type_, base_index_type_) \
    linkage_ bool name_##_search(const name_##_t *set, key_type_ key, index_type_ *pos);

#define MAKE_SIMPLESET_ALIAS_SPEC_contains(linkage_, name_, base_name_, key_type_, type_, index_type_, base_type_, base_index_type_) \
    linkage_ bool name_##_contains(const name_##_t *set, key_type_ elem);

#define MAKE_SIMPLESET_ALIAS_SPEC_coount(linkage_, name_, base_name_, key_type_, type_, index_type_, base_type_, base_index_type_) \
    linkage_ index_type_ name_##_count(const name_##_t *set);

#define MAKE_SIMPLESET_ALIAS_SPEC_insert(linkage_, name_, base_name_, key_type_, type_, index_type_, base_type_, base_index_type_) \
    linkage_ simpleset_insert_result_t name_##_insert(name_##_t *set, type_ elem);

#define MAKE_SIMPLESET_ALIAS_SPEC_delete(linkage_, name_, base_name_, key_type_, type_, index_type_, base_type_, base_index_type_) \
    linkage_ bool name_##_delete(name_##_t *set, type_ elem);

#define MAKE_SIMPLESET_ALIAS_SPEC_iter_first(linkage_, name_, base_name_, key_type_, type_, index_type_, base_type_, base_index_type_) \
    linkage_ bool name_##_iter_first(name_##_iter_t *it, const name_##_t *set, type_ *elem);

#define MAKE_SIMPLESET_ALIAS_SPEC_iter_next(linkage_, name_, base_name_, key_type_, type_, index_type_, base_type_, base_index_type_) \
    linkage_ bool name_##_iter_next(name_##_iter_t *it, type_ *elem);


#define MAKE_SIMPLESET_ALIAS_BODY_init(linkage_, name_, base_name_, key_type, type_, index_type_) \
    linkage_ void name_##_init(name_##_t *set) {                        \
        zhe_base_name_##_init(&set->x_##base_name_);                    \
    }

#define MAKE_SIMPLESET_ALIAS_BODY_search(linkage_, name_, base_name_, key_type, type_, index_type_) \
    linkage_ bool name_##_search(const name_##_t *set, key_type_ key, index_type_ *pos) { \
        return zhe_base_name_##_search(&set->x_##base_name_, key, pos); \
    }

#define MAKE_SIMPLESET_ALIAS_BODY_contains(linkage_, name_, base_name_, key_type, type_, index_type_) \
    linkage_ bool name_##_contains(const name_##_t *set, key_type_ elem) { \
        return zhe_base_name_##_contains(&set->x_##base_name_, elem);   \
    }

#define MAKE_SIMPLESET_ALIAS_BODY_count(linkage_, name_, base_name_, key_type, type_, index_type_) \
    linkage_ index_type_ name_##_count(const name_##_t *set) {          \
        return zhe_base_name_##_count(&set->x_##base_name_);            \
    }

#define MAKE_SIMPLESET_ALIAS_BODY_insert(linkage_, name_, base_name_, key_type, type_, index_type_) \
    linkage_ simpleset_insert_result_t name_##_insert(name_##_t *set, type_ elem) { \
        return zhe_base_name_##_insert(&set->x_##base_name_, elem);     \
    }

#define MAKE_SIMPLESET_ALIAS_BODY_delete(linkage_, name_, base_name_, key_type, type_, index_type_) \
    linkage_ bool name_##_delete(name_##_t *set, type_ elem) {          \
        return zhe_base_name_##_delete(&set->x_##base_name_, elem);     \
    }

#define MAKE_SIMPLESET_ALIAS_BODY_iter_first(linkage_, name_, base_name_, key_type, type_, index_type_) \
    linkage_ bool name_##_iter_first(name_##_iter_t *it, const name_##_t *set, type_ *elem) { \
        return zhe_base_name_##_iter_first(&it, &set->x_##base_name_, elem); \
    }

#define MAKE_SIMPLESET_ALIAS_BODY_iter_next(linkage_, name_, base_name_, key_type, type_, index_type_) \
    linkage_ bool name_##_iter_next(name_##_iter_t *it, type_ *elem) {  \
        return zhe_base_name_##_iter_next(&it->x_##base_name_, elem);   \
    }

#endif
