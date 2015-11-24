/*
 * Copyright (C) 2015 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    core_sched Scheduler
 * @ingroup     core
 * @brief       The RIOT scheduler
 * @details
 *
 * @{
 *
 * @file
 * @brief       Add definitions required on the native board
 *
 * @author      Raphael Hiesgen <raphael.hiesgen@haw-hamburg.de>
 */

#ifndef NATIVE_SCHEDULER_H
#define NATIVE_SCHEDULER_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef BOARD_NATIVE
#include <stdio.h>

/*
 * Required to use some C++11 headers with g++ on the native board.
 */
#define __CPU_SETSIZE 1024
#define __NCPUBITS (8* sizeof(__cpu_mask))
typedef unsigned long int __cpu_mask;
typedef struct {
    __cpu_mask __bits[__CPU_SETSIZE / __NCPUBITS];
} cpu_set_t;

/**
 * @brief In all test the function has never been called, hence it is empty for now.
 */
inline int sched_yield(void)
{
    puts("[ERROR] sched_yield called (defined in sched.h)\n");
    return 0;
}
#endif /* BOARD_NATIVE */

#ifdef __cplusplus
}
#endif

#endif /* NATIVE_SCHEDULER_H */
/** @} */
