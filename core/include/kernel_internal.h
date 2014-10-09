/*
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup  core_internal
 * @{
 *
 * @file        kernel_internal.h
 * @brief       prototypes for kernel internal functions
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 */

#ifndef KERNEL_INTERNAL_H_
#define KERNEL_INTERNAL_H_

#ifdef __cplusplus
 extern "C" {
#endif

#include "attributes.h"

/**
 * @brief   Initializes scheduler and creates main and idle task
 */
void kernel_init(void);

/**
 * @brief   Optionally: initializes platform specifics (devices, pin configuration etc.)
 */
void board_init(void);

typedef void *(*thread_task_func_t)(void *arg);

/**
 * @brief   Gets called upon thread creation to set CPU registers
 *
 * @param[in] task_func     First function to call within the thread
 * @param[in] arg           Argument to supply to task_func
 * @param[in] stack_start   Start address of the stack
 * @param[in] stack_size    Stack size
 *
 * @return stack pointer
 */
char *thread_stack_init(thread_task_func_t task_func, void *arg, void *stack_start, int stack_size);

/**
 * @brief  Removes thread from scheduler and set status to #STATUS_STOPPED
 */
NORETURN void sched_task_exit(void);

/**
 * @brief   Prints human readable, ps-like thread information for debugging purposes
 */
void thread_print_stack(void);

#ifdef __cplusplus
}
#endif

#endif /* KERNEL_INTERNAL_H_ */
/** @} */
