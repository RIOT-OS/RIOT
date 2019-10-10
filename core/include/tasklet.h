/*
 * Copyright (C) 2019 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    core_tasklet  Tasklets
 * @ingroup     core
 * @{
 *
 * @file
 * @brief       Support for kernel tasklets
 *
 * A tasklet is a function that is scheduled to run at a kernel-determined
 * safe time. When scheduled, the OS will try to run the task as soon as possible.
 *
 * They might be scheduled several times, but will only run once
 * (after that it's possible to schedule the same task again).
 * Scheduling tasks is a thread safe operation.
 *
 * Internally the OS creates a thread with the highest priority that listens
 * to a thread flag with type @ref THREAD_FLAG_TASKLET.
 *
 * Tasklets are ideal for offloading ISR (radio drivers, timers, etc).
 *
 * Tasklets are not ISR nor thread safe (although they run in the highest
 * priority). Thus, synchronization between a module and its tasklets is up to
 * the implementer.
 *
 * This API is optional and must be enabled by adding "core_tasklet" to USEMODULE.
 *
 * The current implementation doesn't support (yet) prorities for different
 * tasklets.
 *
 * @author      Jose I. Alamos <jose.alamos@haw-hamburg.de>
 */

#ifndef TASKLET_H
#define TASKLET_H

#include <string.h>

#include "clist.h"

/**
 * @brief Stack size of the tasklets thread
 */
#ifndef THREAD_STACKSIZE_TASKLET
#define THREAD_STACKSIZE_TASKLET (THREAD_STACKSIZE_DEFAULT)
#endif

#ifdef __cplusplus
 extern "C" {
#endif

 /**
  * @brief  Tasklet descriptor
  */
typedef struct {
    clist_node_t tasklet_node; /**< circular list node of the tasklet */
    void (*task)(void *arg);   /**< the function to be called when processing the tasklet */
    void *arg;                 /**< the argument of the task */
} tasklet_t;

/**
 * @brief   Init a tasklet with a given function and argument.
 *
 * @param[in] tasklet   Tasklet to be initialized.
 * @param[in] task      Function to be called when processing the tasklet.
 * @param[in] arg       The argument of the task.
 */
static inline void tasklet_init(tasklet_t *tasklet, void (*task)(void *arg), void *arg)
{
    memset(tasklet, '\0', sizeof(tasklet_t));
    tasklet->task = task;
    tasklet->arg = arg;
}

/**
 * @brief   Schedule a tasklet.
 *
 * The task function assigned to @p tasklet will be called as soon as possible
 * in kernel thread context. It's possible to schedule a task from within an
 * Interrupt Service Routine.
 *
 * This function is thread safe.
 *
 * @note    A tasklet must be initialized with @ref tasklet_init!.
 *
 * @param[in] tasklet   Tasklet to be scheduled.
 */
void tasklet_schedule(tasklet_t *tasklet);

/**
 * @brief   Create the tasklet thread.
 *
 *          This function is internally called by the kernel in order to
 *          initialize the tasklet mechanism.
 */
void tasklet_thread_create(void);

/**
 * @brief   Pop a tasklet from the tasklet FIFO list.
 *
 *          This function is thread safe.
 *
 * @return  First (leftmost) element of the tasklet list.
 * @return  NULL if the list is empty.
 */
tasklet_t *tasklet_pop(void);

/**
 * @brief   Add a tasklet to the tasklet list.
 *
 *          This function is thread safe
 *
 * @note    This function is exposed only for testing reasons. To schedule
 *          a tasklet, use @ref tasklet_schedule instead.
 *
 * @param[in] tasklet   Tasklet to be added.
 */
void tasklet_add(tasklet_t *tasklet);

/**
 * @brief   Reset the tasklet list
 *
 * @note    This function is exposed only for testing reasons. It shouldn't be
 *          called outside of the kernel!
 */
void tasklet_list_reset(void);

#ifdef __cplusplus
}
#endif

#endif /* TASKLET_H */
/** @} */
