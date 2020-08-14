/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_uwb_core
 * @{
 *
 * @file
 * @brief       uwb-core DPL (Decawave Porting Layer) thread/task wrappers
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 * @}
 */

#ifndef DPL_DPL_TASKS_H
#define DPL_DPL_TASKS_H

#include "dpl_types.h"

#include "kernel_types.h"
#include "thread.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief dpl task wrapper
 */
struct dpl_task {
    kernel_pid_t pid;   /**< the process id */
};

/**
 * @brief dpl task function
 */
typedef thread_task_func_t dpl_task_func_t;

/**
 * @brief Initialize a task.
 *
 * This function initializes the task structure pointed to by t,
 * clearing and setting it's stack pointer, provides sane defaults
 * and sets the task as ready to run, and inserts it into the operating
 * system scheduler.
 *
 * @param[in]   t               the task to initialize
 * @param[in]   name            task name
 * @param[in]   func            task function to call
 * @param[in]   arg             argument to pass in task init function
 * @param[in]   prio            task priority
 * @param[in]   sanity_itvl     UNUSED
 * @param[in]   stack_bottom    pointer to bottom of the stack
 * @param[in]   stack_size      task stack size
 *
 * @return 0 on success, non-zero on failure.
 */
int dpl_task_init(struct dpl_task *t, const char *name, dpl_task_func_t func,
                  void *arg, uint8_t prio, dpl_time_t sanity_itvl,
                  dpl_stack_t *stack_bottom, uint16_t stack_size);

/**
 * @brief removes specified task
 *
 * NOTE: This interface is currently experimental and not ready for common use
 */
int dpl_task_remove(struct dpl_task *t);

/**
 * @brief Return the number of tasks initialized.
 *
 * @return number of tasks initialized
 */
uint8_t dpl_task_count(void);

/**
 * @brief   Lets current thread yield.
 */
void dpl_task_yield(void);

#ifdef __cplusplus
}
#endif

#endif /* DPL_DPL_TASKS_H */
