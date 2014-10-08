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
 * @file        attributes.h
 * @brief       Compiler attributes/pragmas configuration
 *
 * @author      René Kijewski <rene.kijewski@fu-berlin.de>
 */

#ifndef ATTRIBUTES_H_
#define ATTRIBUTES_H_

#ifdef __cplusplus
 extern "C" {
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

#ifdef __cplusplus
}
#endif

#endif /* ATTRIBUTES_H_ */
/** @} */
