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
#include "arch/thread_arch.h"

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
 * @return  returns <0 on error, pid of newly created task else.
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
