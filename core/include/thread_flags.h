/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    core_thread_flags Thread Flags
 * @ingroup     core
 * @brief       Thread flags
 *
 * This API can be used to notify threads of conditions in a race-free
 * and allocation-less way.
 *
 * Each thread can handle up to 16 boolean flags, stored as a bitmask in the
 * flags field of its thread.  Those flags can be set or unset, using
 * thread_flags_set(), from ISR's, other threads or even by the thread itself.
 *
 * A thread can wait for any combination of its flags to become set, using
 * thread_flags_wait_any() or thread_flags_wait_all().
 * Those functions clear flags that caused them to return.
 * It is not possible to wait for flags to become unset.
 *
 * Thread flags have certain properties that make them the preferred choice
 * over messages or mutexes in some circumstances:
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
 * Usually, if it is only of interest that an event occurred, but not how many
 * of them, thread flags should be considered.
 *
 * Note that some flags (currently the three most significant bits) are used by
 * core functions and should not be set by the user. They can be waited for.
 *
 * This API is optional and must be enabled by adding "core_thread_flags" to USEMODULE.
 *
 * @{
 *
 * @file
 * @brief       Thread Flags API
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */
#ifndef THREAD_FLAGS_H
#define THREAD_FLAGS_H

#ifndef MODULE_CORE_THREAD_FLAGS
#error Missing USEMODULE += core_thread_flags
#endif

#include "kernel_types.h"
#include "sched.h"  /* for thread_t typedef */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name reserved thread flags
 * @{
 */

/**
 * @brief Set when a message becomes available
 *
 * This flag is set for a thread if a message becomes available either by being
 * queued in the thread's message queue or by another thread becoming
 * send-blocked.
 *
 * It is only reset through thread_flags_wait_*(), not by msg_receive().
 *
 * One thread flag event might point to one, many or no messages ready to be
 * received.  It is advisable to use the non-blocking @ref msg_try_receive() in
 * a loop to retrieve the messages.
 * Use e.g., the following pattern when waiting for both thread flags and
 * messages:
 *
 *      while(1) {
 *          thread_flags_t flags = thread_flags_wait_any(0xFFFF);
 *          if (flags & THREAD_FLAG_MSG_WAITING) {
 *              msg_t msg;
 *              while (msg_try_receive(&msg) == 1) {
 *                  [ handle msg ]
 *              }
 *          }
 *          ...
 *      }
 */
#define THREAD_FLAG_MSG_WAITING     (1u << 15)
/**
 * @brief Set by @ref xtimer_set_timeout_flag() when the timer expires
 *
 * @see xtimer_set_timeout_flag
 */
#define THREAD_FLAG_TIMEOUT         (1u << 14)
/** @} */

/**
 * @brief Type definition of thread_flags_t
 */
typedef uint16_t thread_flags_t;

/**
 * @brief Set thread flags, possibly waking it up
 *
 * @param[in]   thread     thread to work on
 * @param[in]   mask    additional flags to be set for the current thread,
 *                      represented as a bitmask
 */
void thread_flags_set(thread_t *thread, thread_flags_t mask);

/**
 * @brief Clear current thread's flags
 *
 * @param[in]   mask    unset flags for the current thread,
 *                      represented as a bitmask
 *
 * @returns     flags that have actually been cleared (mask & thread->flags before clear)
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
 *
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
 * one flag at a time, least significant set bit first.
 *
 * @param[in]   mask    mask of flags to wait for
 *
 * @returns     flag that triggered the return / wait
 */
thread_flags_t thread_flags_wait_one(thread_flags_t mask);

/**
 * @brief Possibly Wake up thread waiting for flags
 *
 * Wakes up a thread if it is thread flag blocked and its condition is met.
 * Has to be called with interrupts disabled.
 * Does not trigger yield.
 *
 * @internal
 *
 * @param[in]   thread  thread to possibly wake up
 * @return      1       if @p thread has been woken up
 *              0       otherwise
 */
int thread_flags_wake(thread_t *thread);

#ifdef __cplusplus
}
#endif

#endif /* THREAD_FLAGS_H */
/** @} */
