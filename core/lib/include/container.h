/*
 * Copyright (C) 2014 Freie Universität Berlin
 *               2017 HAW-Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     core_internal
 * @{
 *
 * @file
 * @brief       Common macros and compiler attributes/pragmas configuration
 *
 * @author      René Kijewski <rene.kijewski@fu-berlin.de>
 * @author      Michel Rottleuthner <michel.rottleuthner@haw-hamburg.de>
 */

#ifndef CONTAINER_H
#define CONTAINER_H

#include <stddef.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* uncrustify gets mightily confused by these macros... */
/* begin{code-style-ignore} */

/**
 * @def         container_of(PTR, TYPE, MEMBER)
 * @brief       Returns the container of a pointer to a member.
 * @details     For a struct `TYPE` with a member `MEMBER`,
 *              given a pointer `PTR` to `TYPE::MEMBER` this function returns a pointer
 *              to the instance of `TYPE`.
 * @details     E.g. for `struct my_struct_t { ...; something_t n; ... } my_struct;`,
 *              `&my_struct == container_of(&my_struct.n, struct my_struct_t, n)`.
 * @param[in]   PTR      pointer to a member
 * @param[in]   TYPE     a type name (a struct or union), container of PTR
 * @param[in]   MEMBER   name of the member of TYPE which PTR points to
 * @return      Pointer to the container of PTR.
 */
#if __STDC_VERSION__ >= 201112L
#   define container_of(PTR, TYPE, MEMBER) \
        (_Generic((PTR), \
            const __typeof__ (((TYPE *) 0)->MEMBER) *: \
                ((TYPE *) ((uintptr_t) (PTR) - offsetof(TYPE, MEMBER))), \
            __typeof__ (((TYPE *) 0)->MEMBER) *: \
                ((TYPE *) ((uintptr_t) (PTR) - offsetof(TYPE, MEMBER))) \
        ))
#elif defined __GNUC__
#   define container_of(PTR, TYPE, MEMBER) \
        (__extension__ ({ \
            __extension__ const __typeof__ (((TYPE *) 0)->MEMBER) *__m____ = (PTR); \
            ((TYPE *) ((uintptr_t) __m____ - offsetof(TYPE, MEMBER))); \
        }))
#else
#   define container_of(PTR, TYPE, MEMBER) \
        ((TYPE *) ((char *) (PTR) - offsetof(TYPE, MEMBER)))
#endif

/**
 * @def         index_of(ARRAY, ELEMENT)
 * @brief       Returns the index of a pointer to an array element.

 * @param[in]   ARRAY    an array
 * @param[in]   ELEMENT  pointer to an array element
 * @return      Index of the element in the array
 */
#define index_of(ARRAY, ELEMENT) (((uintptr_t)(ELEMENT) - (uintptr_t)(ARRAY)) / sizeof((ARRAY)[0]))

/**
 * @def ARRAY_SIZE(a)
 * @brief       Calculate the number of elements in a static array.
 * @param[in]   a   Array to examine
 * @returns     The number of elements in the array a.
 */
#ifndef ARRAY_SIZE
#define ARRAY_SIZE(a) (sizeof((a)) / sizeof((a)[0]))
#endif

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* CONTAINER_H */
