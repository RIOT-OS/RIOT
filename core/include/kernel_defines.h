/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup  core_internal
 * @{
 *
 * @file
 * @brief       Common macros and compiler attributes/pragmas configuration
 *
 * @author      René Kijewski <rene.kijewski@fu-berlin.de>
 */

#ifndef KERNEL_DEFINES_H_
#define KERNEL_DEFINES_H_

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

/**
 * @def NORETURN
 * @brief The *NORETURN* keyword tells the compiler to assume that the function
 *        cannot return.
 */
#ifdef __GNUC__
#define NORETURN  __attribute__((noreturn))
#else
#define NORETURN
#endif

/**
 * @def CONST
 * @brief A function declared as *CONST* is #PURE and also not allowed to
 *        examine global memory. I.e. a *CONST* function cannot even
 *        dereference a pointer parameter.
 */
#ifdef __GNUC__
#define CONST  __attribute__((const))
#else
#define CONST
#endif

/**
 * @def PURE
 * @brief The function has no effects except the return value and its return
 *        value depends only on the parameters and/or global variables. Such a
 *        function can be subject to common subexpression elimination and loop
 *        optimization just as an arithmetic operator would be.
 */
#ifdef __GNUC__
#define PURE  __attribute__((pure))
#else
#define PURE
#endif

/**
 * @def       UNREACHABLE()
 * @brief     Tell the compiler that this line of code cannot be reached.
 * @details   Most useful in junction with #NORETURN.
 *            Use this if the compiler cannot tell that e.g.
 *            an assembler instruction causes a longjmp, or a write causes a reboot.
 */
#if ((__GNUC__ == 4) && (__GNUC_MINOR__ >= 5)) || (__GNUC__ >= 5)
#define UNREACHABLE() __builtin_unreachable()
#else
#define UNREACHABLE() do { /* nothing */ } while (1)
#endif

/**
 * @def         ALIGN_OF(T)
 * @brief       Calculate the minimal alignment for type T.
 * @param[in]   T   Type to examine
 * @returns     The minimal alignment of T.
 */
#define ALIGN_OF(T) (offsetof(struct { char c; T t; }, t))

#ifdef __cplusplus
}
#endif

#endif /* KERNEL_DEFINES_H_ */
/** @} */
