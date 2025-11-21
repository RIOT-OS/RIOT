/*
 * SPDX-FileCopyrightText: 2015 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    core_sched_native Scheduler for native
 * @ingroup     core_sched
 * @brief       The RIOT scheduler for the native platform
 * @details
 *
 * @{
 *
 * @file
 * @brief       Add definitions required on the native board
 *
 * @author      Raphael Hiesgen <raphael.hiesgen@haw-hamburg.de>
 */

#ifdef __cplusplus
extern "C" {
#endif

#ifdef CPU_NATIVE
#include <stdio.h>

#if __GLIBC__
/*
 * Required to use some C++11 headers with g++ on the native board.
 */
#define __CPU_SETSIZE 1024
#define __NCPUBITS (8 * sizeof(__cpu_mask))
typedef unsigned long int __cpu_mask;
typedef struct {
    __cpu_mask __bits[__CPU_SETSIZE / __NCPUBITS];
} cpu_set_t;
#endif

/**
 * @brief In all test the function has never been called, hence it is empty for now.
 */
inline int sched_yield(void)
{
    puts("[ERROR] sched_yield called (defined in sched.h)\n");
    return 0;
}
#else
/**
 * @brief Compilation with g++ may require the declaration of this function.
 *
 * If implementation of this function is required, it can be realized in
 * thread_arch.c.
 */
extern int sched_yield(void);
#endif /* CPU_NATIVE */

#ifdef __cplusplus
}
#endif

/** @} */
