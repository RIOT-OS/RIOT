/*
 * Copyright (C) 2020 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_mynewt_core
 * @{
 *
 * @file
 * @brief       Decawave Porting Layer tasks RIOT wrapper
 *
 * @author      Francisco Molina <francois-xavier.molina@inria.fr>
 * @}
 */

#include "os/os_error.h"
#include "os/os_task.h"
#include "thread.h"

#include "log.h"

#ifdef __cplusplus
extern "C" {
#endif

int os_task_init(struct os_task *t, const char *name, os_task_func_t func,
                  void *arg, uint8_t prio, os_time_t sanity_itvl,
                  os_stack_t *stack_bottom, uint16_t stack_size)
{
    (void) sanity_itvl;

    LOG_DEBUG("[mynewt-core]: starting thread %s\n", name);

    kernel_pid_t pid = thread_create(stack_bottom, (int) stack_size,
                                     prio, THREAD_CREATE_STACKTEST,
                                     func, arg, name);

    t->pid = pid;

    return (pid) ? OS_ERROR : OS_OK;;
}

int os_task_remove(struct os_task *t)
{
    thread_zombify();
    return thread_kill_zombie(t->pid);
}

uint8_t os_task_count(void)
{
    return sched_num_threads;
}

void os_task_yield(void)
{
    thread_yield();
}

#ifdef __cplusplus
}
#endif
