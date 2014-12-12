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
 * @details     If ENABLE_DEBUG is set, before this header is included, 
 *              ::DEBUG will print out to stdout, otherwise do nothing
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef __DEBUG_H
#define __DEBUG_H

#include <stdio.h>
#include "sched.h"

#ifdef __cplusplus
 extern "C" {
#endif

/**
 * @name Print debug information if the calling thread stack is large enough
 *
 * Use this macro the same as `printf`. When `DEVELHELP` is defined inside an
 * implementation file, all usages of `DEBUG_PRINT` will print the given
 * information to std-out after verifying the stack is big enough. If `DEVELHELP`
 * is not set, this check is not performed. (CPU exception may occur)
 *
 * @{
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
/** @} */

/**
 * @brief Print debug information to std-out
 *
 * If *ENABLE_DEBUG* is defined inside an implementation file, all calls to
 * *DEBUG* and *DEBUGF* will work the same as *printf* and output the given
 * information to stdout. If *ENABLE_DEBUG* is not defined, all calls to
 * *DEBUG* and *DEBUGF* will be ignored.
 *
 * In addition to just printing the given information *DEBUGF* will further
 * print extended debug information about the current thread and function.
 *
 * @{
 */
#if ENABLE_DEBUG
#include "tcb.h"
#define DEBUG(...) DEBUG_PRINT(__VA_ARGS__)
#define DEBUGF(...) \
    do { \
        DEBUG_PRINT("DEBUG(%s): %s:%d in %s: ", \
                sched_active_thread ? sched_active_thread->name : "NO THREAD", \
                __FILE__, __LINE__, __func__); \
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

#endif /* __DEBUG_H */
/** @} */
