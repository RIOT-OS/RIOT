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
 * @file
 * @brief       Debug-header
 *
 * @details     If *ENABLE_DEBUG* is defined inside an implementation file, all
 *              calls to ::DEBUG will work the same as *printf* and output the
 *              given information to stdout. If *ENABLE_DEBUG* is not defined,
 *              all calls to ::DEBUG will be ignored.
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>
#include "sched.h"
#include "thread.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @def ENABLE_DEBUG
 * @brief   This macro can be defined as 0 or other on a file-based level.
 *          @ref DEBUG() will generate output only if ENABLE_DEBUG is non-zero.
 */

/**
 * @def DEBUG_PRINT
 *
 * @brief Print debug information if the calling thread stack is large enough
 *
 * Use this macro the same as `printf`. When `DEVELHELP` is defined inside an
 * implementation file, all usages of ::DEBUG_PRINT will print the given
 * information to stdout after verifying the stack is big enough. If `DEVELHELP`
 * is not set, this check is not performed. (CPU exception may occur)
 */
#ifdef DEVELHELP
#include "cpu_conf.h"
#define DEBUG_PRINT(...) \
    do { \
        if ((sched_active_thread == NULL) || (sched_active_thread->stack_size >= THREAD_EXTRA_STACKSIZE_PRINTF)) { \
            printf(__VA_ARGS__); \
        } \
        else { \
            puts("Cannot debug, stack too small"); \
        } \
    } while (0)
#else
#define DEBUG_PRINT(...) printf(__VA_ARGS__)
#endif

/**
 * @name Debugging defines
 * @{
 */
#ifndef ENABLE_DEBUG
#define ENABLE_DEBUG (0)
#endif

/**
 * @def DEBUG_FUNC
 *
 * @brief   Contains the function name if given compiler supports it.
 *          Otherwise it is an empty string.
 */
# if defined(__cplusplus) && defined(__GNUC__)
#  define DEBUG_FUNC __PRETTY_FUNCTION__
# elif __STDC_VERSION__ >= 199901L
#  define DEBUG_FUNC __func__
# elif __GNUC__ >= 2
#  define DEBUG_FUNC __FUNCTION__
# else
#  define DEBUG_FUNC ""
# endif

/**
 * @def DEBUG
 *
 * @brief Print debug information to stdout
 *
 * @note Another name for ::DEBUG_PRINT
 */
#define DEBUG(...) if (ENABLE_DEBUG) DEBUG_PRINT(__VA_ARGS__)
/** @} */

/**
 * @def DEBUG_EXTRA_STACKSIZE
 *
 * @brief Extra stacksize needed when ENABLE_DEBUG==1
 */
#if ENABLE_DEBUG
#define DEBUG_EXTRA_STACKSIZE THREAD_EXTRA_STACKSIZE_PRINTF
#else
#define DEBUG_EXTRA_STACKSIZE (0)
#endif

#ifdef __cplusplus
}
#endif

#endif /* DEBUG_H */
/** @} */
