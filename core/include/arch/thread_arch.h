/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     core_arch
 * @{
 *
 * @file
 * @brief       Architecture dependent kernel interface for handling and managing threads
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef THREAD_ARCH_H
#define THREAD_ARCH_H

#include "kernel_defines.h"

#ifdef __cplusplus
 extern "C" {
#endif

/**
 * @name Define the mapping between the architecture independent interfaces
 *       and the kernel internal interfaces
 *
 * This mapping is done for compatibility of existing platforms,
 * new platforms should always use the *_arch_* interfaces.
 * @{
 */
#ifdef COREIF_NG
#define thread_stack_init               thread_arch_stack_init
#define thread_print_stack              thread_arch_print_stack
#define cpu_switch_context_exit         thread_arch_start_threading
#define thread_yield_higher             thread_arch_yield
#endif
/** @} */

/**
 * @brief Prototype for a thread entry function
 */
typedef void *(*thread_task_func_t)(void *arg);

/**
 * @brief Initialize a thread's stack
 *
 * @param[in] task_func     pointer to the thread's code
 * @param[in] arg           argument to task_func
 * @param[in] stack_start   pointer to the start address of the thread
 * @param[in] stack_size    the maximum size of the stack
 *
 * @return                  pointer to the new top of the stack
 */
char *thread_arch_stack_init(thread_task_func_t task_func, void *arg, void *stack_start, int stack_size);

/**
 * @brief Print the current stack to stdout
 */
void thread_arch_stack_print(void);

/**
 * @brief Start threading by loading a threads initial information from the stack
 */
void thread_arch_start_threading(void) NORETURN;

/**
 * @brief Pause the current thread and schedule the next pending, if available
 */
void thread_arch_yield(void);

#ifdef __cplusplus
}
#endif

#endif /* THREAD_ARCH_H */
/** @} */
