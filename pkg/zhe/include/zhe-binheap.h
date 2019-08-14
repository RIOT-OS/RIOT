#ifndef ZHE_BINHEAP_PKG_H
#define ZHE_BINHEAP_PKG_H

#include <stdbool.h>
#include "zhe-assert.h"
#include "zhe-package.h"

#define MAKE_BINHEAP_SPEC_type(linkage_, name_, key_type_, elem_type_, index_type_, max_elems_) \
    typedef struct name_ {                                              \
        index_type_ n;                                                  \
        key_type_ ks[max_elems_];                                       \
        elem_type_ vs[max_elems_];                                      \
        index_type_ ix[max_elems_];                                     \
    } name_##_t;

#define MAKE_BINHEAP_SPEC_init(linkage_, name_, key_type_, elem_type_, index_type_, max_elems_) \
    linkage_ void name_##_init(name_##_t *h);
#define MAKE_BINHEAP_SPEC_insert(linkage_, name_, key_type_, elem_type_, index_type_, max_elems_) \
    linkage_ void name_##_insert(name_##_t *h, elem_type_ v, key_type_ k);
#define MAKE_BINHEAP_SPEC_delete(linkage_, name_, key_type_, elem_type_, index_type_, max_elems_) \
    linkage_ bool name_##_delete(name_##_t *h, elem_type_ v);
#define MAKE_BINHEAP_SPEC_isempty(linkage_, name_, key_type_, elem_type_, index_type_, max_elems_) \
    linkage_ bool name_##_isempty(const name_##_t *h);
#define MAKE_BINHEAP_SPEC_raisekey(linkage_, name_, key_type_, elem_type_, index_type_, max_elems_) \
    linkage_ key_type_ name_##_raisekey(name_##_t *h, elem_type_ v, key_type_ nk, key_type_ k_if_discarded);
#define MAKE_BINHEAP_SPEC_min(linkage_, name_, key_type_, elem_type_, index_type_, max_elems_) \
    linkage_ key_type_ name_##_min(const name_##_t *h);

#define MAKE_BINHEAP_BODY_init(linkage_, name_, key_type_, elem_type_, invalid_elem_, index_type_, lt_, max_elems_) \
    linkage_ void name_##_init(name_##_t *h)                            \
    {                                                                   \
        h->n = 0;                                                       \
        for (elem_type_ j = 0; j < max_elems_; j++) {                   \
            h->vs[j] = invalid_elem_;                                   \
            h->ix[j] = invalid_elem_;                                   \
        }                                                               \
    }

#define MAKE_BINHEAP_BODY_heapify(linkage_, name_, key_type_, elem_type_, invalid_elem_, index_type_, lt_, max_elems_) \
    linkage_ void name_##_heapify(name_##_t *h, index_type_ j)          \
    {                                                                   \
        index_type_ k;                                                  \
        /* j < n/2 term to protect against overflow of k:               \
           - for n even: j < n/2 => k < 2*(n/2)+1 = n+1                 \
           - for n odd:  j < n/2 = (n-1)/2 => k < 2*((n-1)/2)+1 = n     \
           n+1 potentially overflows, but j < n/2 takes care of that    \
           --                                                           \
           in the loop body k can only be incremented if k+1 < n,       \
           but (1) k < n and (2) n is in range, so k+1 is               \
           at most n and therefore also in range */                     \
        for (k = 2*j+1; j < h->n/2 && k < h->n; j = k, k += k+1) {      \
          if (k+1 < h->n && lt_(h->ks[h->vs[k+1]], h->ks[h->vs[k]])) {  \
                k++;                                                    \
            }                                                           \
            if (lt_(h->ks[h->vs[k]], h->ks[h->vs[j]])) {                \
                elem_type_ t;                                           \
                t = h->vs[j]; h->vs[j] = h->vs[k]; h->vs[k] = t;        \
                h->ix[h->vs[j]] = j; h->ix[h->vs[k]] = k;               \
            }                                                           \
        }                                                               \
    }

#ifndef NDEBUG
#define MAKE_BINHEAP_BODY_check(linkage_, name_, key_type_, elem_type_, invalid_elem_, index_type_, lt_, max_elems_) \
    linkage_ void name_##_check(const name_##_t *h)                     \
    {                                                                   \
        index_type_ cnt = 0;                                            \
        zhe_assert(h->n <= max_elems_);                                 \
        for (index_type_ j = 0; j < max_elems_; j++) {                  \
            zhe_assert(h->ix[j] == invalid_elem_ || (h->ix[j] < h->n && h->vs[h->ix[j]] == j)); \
            cnt += (h->ix[j] != invalid_elem_);                         \
        }                                                               \
        zhe_assert(cnt == h->n);                                        \
        for (index_type_ j = 0; j < h->n/2; j++) {                      \
            index_type_ k = 2*j+1;                                      \
            zhe_assert (k >= h->n || !lt_(h->ks[h->vs[k]], h->ks[h->vs[j]])); \
            zhe_assert (k+1 >= h->n || !lt_(h->ks[h->vs[k+1]], h->ks[h->vs[j]])); \
        }                                                               \
    }
#else
#define MAKE_BINHEAP_BODY_check(linkage_, name_, key_type_, elem_type_, invalid_elem_, index_type_, lt_, max_elems_) \
    linkage_ void name_##_check(const name_##_t *h) { }
#endif

#define MAKE_BINHEAP_BODY_insert(linkage_, name_, key_type_, elem_type_, invalid_elem_, index_type_, lt_, max_elems_) \
    linkage_ void name_##_insert(name_##_t *h, elem_type_ v, key_type_ k) \
    {                                                                   \
        index_type_ i;                                                  \
        name_##_check(h);                                               \
        zhe_assert(h->ix[v] == invalid_elem_);                          \
        h->ks[v] = k;                                                   \
        i = h->n++;                                                     \
        while (i > 0 && lt_(h->ks[v], h->ks[h->vs[(i-1)/2]])) {         \
            h->vs[i] = h->vs[(i-1)/2];                                  \
            h->ix[h->vs[i]] = i;                                        \
            i = (i-1)/2;                                                \
        }                                                               \
        h->vs[i] = v;                                                   \
        h->ix[h->vs[i]] = i;                                            \
        name_##_check(h);                                               \
    }

#define MAKE_BINHEAP_BODY_min(linkage_, name_, key_type_, elem_type_, invalid_elem_, index_type_, lt_, max_elems_) \
    linkage_ key_type_ name_##_min(const name_##_t *h)                  \
    {                                                                   \
        zhe_assert(h->n > 0);                                           \
        return h->ks[h->vs[0]];                                         \
    }

#define MAKE_BINHEAP_BODY_raisekey(linkage_, name_, key_type_, elem_type_, invalid_elem_, index_type_, lt_, max_elems_) \
    linkage_ key_type_ name_##_raisekey(name_##_t *h, elem_type_ v, key_type_ nk, key_type_ k_if_discarded) \
    {                                                                   \
        name_##_check(h);                                               \
        if (h->ix[v] == invalid_elem_ || !lt_(h->ks[v], nk)) {          \
            return k_if_discarded;                                      \
        } else {                                                        \
            zhe_assert(h->vs[h->ix[v]] == v);                           \
            h->ks[v] = nk;                                              \
            name_##_heapify(h, h->ix[v]);                               \
            name_##_check(h);                                           \
            return h->ks[h->vs[0]];                                     \
        }                                                               \
    }

#define MAKE_BINHEAP_BODY_delete(linkage_, name_, key_type_, elem_type_, invalid_elem_, index_type_, lt_, max_elems_) \
    linkage_ bool zhe_minseqheap_delete(name_##_t *h, elem_type_ v)     \
    {                                                                   \
        /* returns (v contained in heap) */                             \
        const index_type_ i = h->ix[v];                                 \
        name_##_check(h);                                               \
        if (i == invalid_elem_) {                                       \
            return 0;                                                   \
        } else {                                                        \
            zhe_assert(h->vs[i] == v);                                  \
            h->ix[v] = invalid_elem_;                                   \
            if (h->n == 1) {                                            \
                h->n = 0;                                               \
            } else {                                                    \
                h->n--;                                                 \
                if (i < h->n) {                                         \
                    h->vs[i] = h->vs[h->n];                             \
                    h->ix[h->vs[i]] = i;                                \
                    name_##_heapify(h, i);                              \
                }                                                       \
                name_##_check(h);                                       \
            }                                                           \
            return 1;                                                   \
        }                                                               \
    }

#define MAKE_BINHEAP_BODY_isempty(linkage_, name_, key_type_, elem_type_, invalid_elem_, index_type_, lt_, max_elems_) \
    linkage_ bool name_##_isempty(const name_##_t *h)                   \
    {                                                                   \
        return h->n == 0;                                               \
    }

#endif
