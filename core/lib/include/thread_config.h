/*
 * Copyright (C) 2014-21 Freie Universität Berlin
 *               2021 Inria
 *               2021 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     core_thread
 *
 * @{
 *
 * @file
 * @brief       Thread configuration defines
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef THREAD_CONFIG_H
#define THREAD_CONFIG_H

#include "cpu_conf.h"
#include "sched.h"      /* for SCHED_PRIO_LEVELS */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @def THREAD_STACKSIZE_DEFAULT
 * @brief A reasonable default stack size that will suffice most smaller tasks
 *
 * @note This value must be defined by the CPU specific implementation, please
 *       take a look at @c cpu/$CPU/include/cpu_conf.h
 */
#ifndef THREAD_STACKSIZE_DEFAULT
#error THREAD_STACKSIZE_DEFAULT must be defined per CPU
#endif
#ifdef DOXYGEN
#define THREAD_STACKSIZE_DEFAULT
#endif

/**
 * @def THREAD_STACKSIZE_IDLE
 * @brief Size of the idle task's stack in bytes
 *
 * @note This value must be defined by the CPU specific implementation, please
 *       take a look at @c cpu/$CPU/include/cpu_conf.h
 */
#ifndef THREAD_STACKSIZE_IDLE
#error THREAD_STACKSIZE_IDLE must be defined per CPU
#endif
#ifdef DOXYGEN
#define THREAD_STACKSIZE_IDLE
#endif

/**
 * @def THREAD_EXTRA_STACKSIZE_PRINTF
 * @brief Size of the task's printf stack in bytes
 *
 * @note This value must be defined by the CPU specific implementation, please
 *       take a look at @c cpu/$CPU/include/cpu_conf.h
 */
#ifndef THREAD_EXTRA_STACKSIZE_PRINTF
#error THREAD_EXTRA_STACKSIZE_PRINTF must be defined per CPU
#endif
#ifdef DOXYGEN
#define THREAD_EXTRA_STACKSIZE_PRINTF
#endif

/**
 * @def THREAD_STACKSIZE_MAIN
 * @brief Size of the main task's stack in bytes
 */
#ifndef THREAD_STACKSIZE_MAIN
#define THREAD_STACKSIZE_MAIN      (THREAD_STACKSIZE_DEFAULT + \
                                    THREAD_EXTRA_STACKSIZE_PRINTF)
#endif

/**
 * @brief Large stack size
 */
#ifndef THREAD_STACKSIZE_LARGE
#define THREAD_STACKSIZE_LARGE (THREAD_STACKSIZE_MEDIUM * 2)
#endif

/**
 * @brief Medium stack size
 */
#ifndef THREAD_STACKSIZE_MEDIUM
#define THREAD_STACKSIZE_MEDIUM THREAD_STACKSIZE_DEFAULT
#endif

/**
 * @brief Small stack size
 */
#ifndef THREAD_STACKSIZE_SMALL
#define THREAD_STACKSIZE_SMALL (THREAD_STACKSIZE_MEDIUM / 2)
#endif

/**
 * @brief Tiny stack size
 */
#ifndef THREAD_STACKSIZE_TINY
#define THREAD_STACKSIZE_TINY (THREAD_STACKSIZE_MEDIUM / 4)
#endif

/**
 * @brief Minimum stack size
 */
#ifndef THREAD_STACKSIZE_MINIMUM
#define THREAD_STACKSIZE_MINIMUM  (sizeof(thread_t))
#endif

/**
 * @def THREAD_PRIORITY_MIN
 * @brief Least priority a thread can have
 */
#define THREAD_PRIORITY_MIN            (SCHED_PRIO_LEVELS - 1)

/**
 * @def THREAD_PRIORITY_IDLE
 * @brief Priority of the idle thread
 */
#define THREAD_PRIORITY_IDLE           (THREAD_PRIORITY_MIN)

/**
 * @def THREAD_PRIORITY_MAIN
 * @brief Priority of the main thread
 */
#ifndef THREAD_PRIORITY_MAIN
#define THREAD_PRIORITY_MAIN           (THREAD_PRIORITY_MIN - \
                                        (SCHED_PRIO_LEVELS / 2))
#endif

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* THREAD_CONFIG_H */
