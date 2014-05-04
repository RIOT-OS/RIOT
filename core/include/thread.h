/*
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    core_thread Threading
 * @ingroup     core
 * @brief       Support for multi-threading
 * @{
 *
 * @file        thread.h
 * @brief       Threading API
 *
 * @author      Freie Universität Berlin, Computer Systems & Telematics
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef __THREAD_H
#define __THREAD_H


#include "kernel.h"
#include "tcb.h"

#define STATUS_NOT_FOUND (-1)

/** Minimum stack size */
#ifndef MINIMUM_STACK_SIZE
#define MINIMUM_STACK_SIZE  (sizeof(tcb_t))
#endif

/**
 * @brief Creates a new thread.
 *
 * @param   stack Lowest address of preallocated stack space
 * @param   stacksize ``sizeof (stack)``
 * @param   flags Extra options, use bitwise-or to supply multiple flags:
 *                * CREATE_WOUT_YIELD: do not call thread_yield() after creating the new thread,
 *                                     even if its priority is higher than the current thread.
 *                * CREATE_SLEEPING: set new thread to sleeping state, thread must be woken up manually.
 *                * CREATE_STACKTEST: initialize stack with values needed for stack overflow testing.
 *
 * @param priority Priority of newly created thread:
 *                 * Lower number means higher priority.
 *                 * 0 means highest possible priority. Lowest priority is `PRIORITY_IDLE-1 == 14`.
 *                 * Use ``PRIORITY_LEAST``, ``PRIORITY_LOW``, ``PRIORITY_MAIN``, ``PRIORITY_HIGH`` or ``PRIORITY_HIGHEST``.
 *                   You can finetune the priority with ``PRIORITY_UP`` and ``PRIORITY_DOWN``,
 *                   e.g. ``PRIORITY_MAIN + PRIORITY_UP`` has a higher priority than ``PRIORITY_MAIN``,
 *                   but is lower than ``PRIORITY_HIGH + PRIORITY_DOWN``.
 *
 * @return  `<0` on error, `pid` of the newly created task otherwise.
 */
int thread_create(char *stack, int stacksize, char priority, int flags, void (*function) (void), const char *name);

/**
 * @brief   returns the status of a process.
 * @return  STATUS_NOT_FOUND if pid is unknown
 */
int thread_getstatus(int pid);

/**
 * @brief   returns the name of a process.
 * @return  NULL if pid is unknown
 */
const char *thread_getname(int pid);

/**
 * @brief   Puts the current thread into sleep mode. Has to be woken up externally.
 */
void thread_sleep(void);

/**
 * @brief   The current thread yields and let the scheduler run.
 */
void thread_yield(void);

/**
 * @brief   Wakes up a sleeping thread.
 * @param   pid The PID of the thread to be woken up
 * @return  STATUS_NOT_FOUND if pid is unknown or not sleeping
 */
int thread_wakeup(int pid);


/**
 * @brief   Returns the process ID of the currently running thread.
 * @return  Obviously you are not a golfer.
 */
int thread_getpid(void);

/**
 * @brief   Returns the process ID of the thread running before the current one.
 * @return  Obviously you are not a golfer.
 */
int thread_getlastpid(void);

/**
 * @brief Measures the stack usage of a stack.
 * Only works if the thread was created with the flag CREATE_STACKTEST.
 *
 * @param stack The stack you want to measure. try active_thread->stack_start.
 * @return  The amount of unused space of the thread's stack
 */
int thread_measure_stack_free(char *stack);

/* @} */
#endif /* __THREAD_H */
