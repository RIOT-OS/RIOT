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
 * @file        debug.h
 * @brief       Debug-header
 *
 * @details     If *ENABLE_DEBUG* is defined inside an implementation file, all
 *              calls to ::DEBUG and ::DEBUGF* will work the same as *printf*
 *              and output the given information to stdout. If *ENABLE_DEBUG*
 *              is not defined, all calls to ::DEBUG and ::DEBUGF will be
 *              ignored.
 *
 *              In addition to just printing the given information ::DEBUGF
 *              will further print extended debug information about the current
 *              thread and function.
     *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef DEBUG_H
#define DEBUG_H

#include <stdio.h>
#include "sched.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @def ENABLE_DEBUG
 * @brief   This macro can be defined as 0 or other on a file-based level.
 *          If ENABLE_DEBUG is 0 @ref DEBUG() and @ref DEBUGF() will generate
 *          no output if not they will generate output.
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
#if DEVELHELP
#include "cpu-conf.h"
#define DEBUG_PRINT(...) \
    do { \
        if ((sched_active_thread == NULL) || (sched_active_thread->stack_size >= KERNEL_CONF_STACKSIZE_PRINTF)) { \
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
 *
 * @{
 */
#if ENABLE_DEBUG
#include "tcb.h"

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
#define DEBUG(...) DEBUG_PRINT(__VA_ARGS__)
/**
 * @def DEBUGF
 *
 * @brief Print extended debug information about the current thread and
 *        function to stdout
 */
#define DEBUGF(...) \
    do { \
        DEBUG_PRINT("DEBUG(%s): %s:%d in %s: ", \
                sched_active_thread ? sched_active_thread->name : "NO THREAD", \
                __FILE__, __LINE__, DEBUG_FUNC); \
        DEBUG_PRINT(__VA_ARGS__); \
    } while (0)
#else
#define DEBUG(...)
#define DEBUGF(...)
#endif
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* DEBUG_H */
/** @} */
