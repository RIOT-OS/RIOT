/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     core_thread
 * @brief       Thread flags
 * @{
 *
 * @file
 * @brief       Thread flags API
 *
 * This API can be used to notify threads of conditions in a race-free
 * and allocation-less way.
 *
 * Each thread has a number boolean flags (usually 16) in it's tcb, stored in
 * it's flags field. Those flags can be set or unset, using thread_flags_set(),
 * from ISR's, other threads or even by the thread itself.
 *
 * A thread can wait for any combination of it's flags to become set, using
 * thread_flags_wait_any() or thread_flags_wait_all().
 * Those functions clear flags that caused them to return.
 * It is not possible to wait for flags to become unset.
 *
 * Thread flags, apart from being very efficient, have certain properties
 * that make them the preferred choice over messages or mutexes in some
 * circumstances:
 *
 * - setting thread flags cannot fail
 *   If messages are used to notify a thread of a condition from within an ISR,
 *   and the receiving thread is not waiting, has no queue or the queue is
 *   full, the ISR cannot deliver the message. A thread flag can always be set.
 *
 * - thread flags are very flexible
 *   With thread flags it is possible to wait for multiple conditions and
 *   messages at the same time. When mutexes are used to notify about events,
 *   only one event can be waited for.
 *
 * Note that some flags (currently the three most significant bits) are used by
 * core functions and should not be set by the user. They can be waited for.
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */
#ifndef THREAD_FLAG_H
#define THREAD_FLAG_H

#include "kernel_types.h"

#ifdef __cplusplus
 extern "C" {
#endif

/**
 * @name reserved thread flags
 * @{
 */
#define THREAD_FLAG_MSG_WAITING      (0x1<<15)
#define THREAD_FLAG_MUTEX_UNLOCKED   (0x1<<14)
#define THREAD_FLAG_TIMEOUT          (0x1<<13)
/** @} */

typedef uint16_t thread_flags_t;
typedef struct tcb tcb_t;

/**
 * @brief Set thread flags, possibly waking it up
 *
 * @param[in]   tcb     thread to work on
 * @param[in]   mask    flags to OR with thread's current flags
 */
void thread_flags_set(tcb_t *tcb, thread_flags_t mask);

/**
 * @brief Clear current thread's flags
 *
 * @param[in]   mask    mask to ~& with thread's current flags
 *
 * @returns     flags that have actually been cleared (mask & tcb->flags before clear)
 */
thread_flags_t thread_flags_clear(thread_flags_t mask);

/**
 * @brief Wait for any flag in mask to become set (blocking)
 *
 * If any of the flags in mask are already set, this function will return
 * immediately, otherwise, it will suspend the thread (as
 * THREAD_STATUS_WAIT_ANY) until any of the flags in mask get set.
 *
 * Both ways, it will clear and return (sched_active_thread-flags & mask).
 *
 * @param[in]   mask    mask of flags to wait for
 *
 * @returns     flags that caused return/wakeup ((sched_active_thread-flags & mask).
 */
thread_flags_t thread_flags_wait_any(thread_flags_t mask);

/**
 * @brief Wait for all flags in mask to become set (blocking)
 
 * If all the flags in mask are already set, this function will return
 * immediately, otherwise, it will suspend the thread (as
 * THREAD_STATUS_WAIT_ALL) until all of the flags in mask have been set.
 *
 * Both ways, it will clear and return (sched_active_thread-flags & mask).
 *
 * @param[in]   mask    mask of flags to wait for
 *
 * @returns     mask
 */
thread_flags_t thread_flags_wait_all(thread_flags_t mask);

/**
 * @brief Wait for any flags in mask to become set (blocking), one at a time
 *
 * This function is like thread_flags_wait_any(), but will only clear and return
 * one flag at a time.
 *
 * @param[in]   mask    mask of flags to wait for
 *
 * @returns     flag that triggered the return / wait
 */
thread_flags_t thread_flags_wait_one(thread_flags_t mask);

/**
 * @brief Possibly wake up thread
 *
 * Has to be called with interrupts disabled.
 * Does not trigger yield.
 *
 * @param[in]   thread  thread to possibly wake up
 * @return      1       if thread has been woken up
 *              0       otherwise
 */
int thread_flags_wake(tcb_t *thread);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* THREAD_FLAG_H */
