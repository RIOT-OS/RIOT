/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup  core_util
 * @{
 *
 * @file        kernel_macros.h
 * @brief       common macros
 *
 * @author      René Kijewski
 */

#include <stddef.h>

#ifdef __cplusplus
extern "C" {
#endif

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
                ((TYPE *) ((char *) (PTR) - offsetof(TYPE, MEMBER))), \
            __typeof__ (((TYPE *) 0)->MEMBER) *: \
                ((TYPE *) ((char *) (PTR) - offsetof(TYPE, MEMBER))) \
        ))
#elif defined __GNUC__
#   define container_of(PTR, TYPE, MEMBER) \
        (__extension__ ({ \
            __extension__ const __typeof__ (((TYPE *) 0)->MEMBER) *__m____ = (PTR); \
            ((TYPE *) ((char *) __m____ - offsetof(TYPE, MEMBER))); \
        }))
#else
#   define container_of(PTR, TYPE, MEMBER) \
        ((TYPE *) ((char *) (PTR) - offsetof(TYPE, MEMBER)))
#endif

#ifdef __cplusplus
}
#endif

/**
 * @}
 */
