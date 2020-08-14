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
 * @brief       Decawave Porting Layer tasks RIOT wrapper
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 * @}
 */

#include "dpl/dpl_error.h"
#include "dpl/dpl_tasks.h"
#include "thread.h"

#ifndef LOG_LEVEL
#define LOG_LEVEL   LOG_INFO
#endif
#include "log.h"

#ifdef __cplusplus
extern "C" {
#endif

int dpl_task_init(struct dpl_task *t, const char *name, dpl_task_func_t func,
                  void *arg, uint8_t prio, dpl_time_t sanity_itvl,
                  dpl_stack_t *stack_bottom, uint16_t stack_size)
{
    (void) sanity_itvl;

    LOG_INFO("dpl: starting thread %s\n", name);

    kernel_pid_t pid = thread_create(stack_bottom, (int) stack_size,
                                     prio, THREAD_CREATE_STACKTEST,
                                     func, arg, name);

    t->pid = pid;

    return (pid) ? DPL_ERROR : DPL_OK;;
}

int dpl_task_remove(struct dpl_task *t)
{
    thread_zombify();
    return thread_kill_zombie(t->pid);
}

uint8_t dpl_task_count(void)
{
    return sched_num_threads;
}

void dpl_task_yield(void)
{
    thread_yield();
}

#ifdef __cplusplus
}
#endif
