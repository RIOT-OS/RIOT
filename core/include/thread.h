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
 * @param   stacksize
 * @param   flags Options:
 * YIELD: force context switch.
 * CREATE_SLEEPING: set new thread to sleeping state, thread must be woken up manually.
 * CREATE_STACKTEST: initialize stack with values needed for stack overflow testing.
 *
 * @param priority Priority of newly created thread. Lower number means higher
 * priority. 0 means highest possible priority. Lowest priority is
 * PRIORITY_IDLE-1, usually 30.
 *
 * @return  returns new thread, or NULL if an error occured.
*/
tcb_t *thread_create(char *stack, int stacksize, char priority, int flags, void (*function) (void), const char *name);

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
 * @param   target the thread to be woken up
 * @return  STATUS_NOT_FOUND if pid is unknown or not sleeping
 */
int thread_wakeup(tcb_t *target);

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
