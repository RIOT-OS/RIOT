/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *               2013, 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    core_sync_mutex Mutex
 * @ingroup     core_sync
 * @brief       Mutex for thread synchronization
 *
 * Mutex Implementation Basics
 * ===========================
 *
 * Data Structures and Encoding
 * ----------------------------
 *
 * A `mutex_t` contains basically a point, which can have one of the following
 * values:
 *
 * 1. `NULL`, in case it is unlocked
 * 2. `MUTEX_LOCKED` in case it is locked, but no other thread is waiting on it
 * 3. A pointer to the head of single linked list of threads (or more precisely
 *    their `thread_t` structures) blocked waiting for obtaining the mutex. This
 *    list is terminated by `NULL`, not by `MUTEX_LOCKED`
 *
 * The same information graphically:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 * Unlocked mutex:
 * +-------+
 * | Mutex | --> NULL
 * +-------+
 *
 * Locked mutex, no waiters:
 * +-------+
 * | Mutex | --> MUTEX_LOCKED
 * +-------+
 *
 * Locked mutex, one waiter:
 * +-------+     +--------+
 * | Mutex | --> | Waiter | --> NULL
 * +-------+     +--------+
 *
 * Locked mutex, 2 waiters:
 * +-------+     +--------+     +--------+
 * | Mutex | --> | Waiter | --> | Waiter | --> NULL
 * +-------+     +--------+     +--------+
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * Obtaining a Mutex
 * -----------------
 *
 * If a `mutex_lock()` is called, one of the following happens:
 *
 * 1. If the mutex was unlocked (value of `NULL`), its value is changed to
 *    `MUTEX_LOCKED` and the call to `mutex_lock()` returns right away without
 *    blocking.
 * 2. If the mutex has a value of `MUTEX_LOCKED`, it will be changed to point to
 *    the `thread_t` of the running thread. The single item list is terminated
 *    be setting the `thread_t::rq_entry.next` of the running thread to `NULL`.
 *    The running thread blocks as described below.
 * 3. Otherwise, the current thread is inserted into the list of waiting
 *    threads sorted by thread priority. The running thread blocks as described
 *    below.
 *
 * In case 2) and 3), the running thread will mark itself as blocked (waiting
 * for a mutex) and yields. Once control is transferred back to this thread
 * (which is done in the call to `mutex_unlock()`), it has the mutex and the
 * function `mutex_lock()` returns.
 *
 * Returning a Mutex
 * -----------------
 *
 * If `mutex_unlock()` is called, one of the following happens:
 *
 * 1. If the mutex was already unlocked (value of `NULL`), the call returns
 *    without modifying the mutex.
 * 2. If the mutex was locked without waiters (value of `MUTEX_LOCKED`), it is
 *    unlocked by setting its value to `NULL`.
 * 3. Otherwise the first `thread_t` from the linked list of waiters is removed
 *    from the list.
 *     - This thread is the one with the highest priority, as the list is sorted
 *       by priority.
 *     - This thread's status is set to pending and its added to the appropriate
 *       run queue.
 *     - If that thread was the last item in the list, the mutex is set to
 *       `MUTEX_LOCK`.
 *     - The scheduler is run, so that if the unblocked waiting thread can
 *       run now, in case it has a higher priority than the running thread.
 * @{
 *
 * @file
 * @brief       Mutex for thread synchronization
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef MUTEX_H
#define MUTEX_H

#include <stddef.h>
#include <stdint.h>

#include "kernel_defines.h"
#include "list.h"
#include "thread.h"

#ifndef __cplusplus
#include "irq.h"
#endif

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Mutex structure. Must never be modified by the user.
 */
typedef struct {
    /**
     * @brief   The process waiting queue of the mutex. **Must never be changed
     *          by the user.**
     * @internal
     */
    list_node_t queue;
} mutex_t;

/**
 * @brief   A cancellation structure for use with @ref mutex_lock_cancelable
 *          and @ref mutex_cancel
 *
 * @note    The contents of this structure are internal.
 */
typedef struct {
    mutex_t *mutex;     /**< The mutex to lock */
    thread_t *thread;   /**< The thread trying to lock the mutex */
    uint8_t cancelled;  /**< Flag whether the mutex has been cancelled */
} mutex_cancel_t;

/**
 * @brief Static initializer for mutex_t.
 * @details This initializer is preferable to mutex_init().
 */
#define MUTEX_INIT { { NULL } }

/**
 * @brief Static initializer for mutex_t with a locked mutex
 */
#define MUTEX_INIT_LOCKED { { MUTEX_LOCKED } }

/**
 * @cond INTERNAL
 * @brief This is the value of the mutex when locked and no threads are waiting
 *        for it
 */
#define MUTEX_LOCKED ((list_node_t *)-1)
/**
 * @endcond
 */

/**
 * @brief   Initializes a mutex object.
 * @details For initialization of variables use MUTEX_INIT instead.
 *          Only use the function call for dynamically allocated mutexes.
 * @param[out]      mutex   pre-allocated mutex structure, must not be NULL.
 */
static inline void mutex_init(mutex_t *mutex)
{
    mutex->queue.next = NULL;
}

/**
 * @brief   Initialize a mutex cancellation structure
 * @param   mutex       The mutex that the calling thread wants to lock
 * @return  The cancellation structure for use with @ref mutex_lock_cancelable
 *          and @ref mutex_cancel
 *
 * @note    This function is considered internal. Out of tree users should be
 *          aware that breaking API changes or removal of this API without
 *          an deprecation period might happen.
 */
static inline mutex_cancel_t mutex_cancel_init(mutex_t *mutex)
{
    mutex_cancel_t result = { mutex, thread_get_active(), 0 };

    return result;
}

/**
 * @brief   Tries to get a mutex, non-blocking.
 *
 * @internal
 * @note    This function is intended for use by languages incompatible
 *          with C (such as C++). Code in C should use @ref mutex_trylock
 *          instead
 *
 * @param[in,out]   mutex   Mutex object to lock.
 *
 * @retval  1               if mutex was unlocked, now it is locked.
 * @retval  0               if the mutex was locked.
 *
 * @pre     @p mutex is not `NULL`
 * @pre     Mutex at @p mutex has been initialized
 * @pre     Must be called in thread context
 */
int mutex_trylock_ffi(mutex_t *mutex);

/**
 * @brief   Tries to get a mutex, non-blocking.
 *
 * @param[in,out]   mutex   Mutex object to lock.
 *
 * @retval  1               if mutex was unlocked, now it is locked.
 * @retval  0               if the mutex was locked.
 *
 * @pre     @p mutex is not `NULL`
 * @pre     Mutex at @p mutex has been initialized
 * @pre     Must be called in thread context
 */
static inline int mutex_trylock(mutex_t *mutex)
{
#ifdef __cplusplus
    return mutex_trylock_ffi(mutex);
#else
    unsigned irq_state = irq_disable();
    int retval = 0;

    if (mutex->queue.next == NULL) {
        mutex->queue.next = MUTEX_LOCKED;
        retval = 1;
    }
    irq_restore(irq_state);
    return retval;
#endif
}

/**
 * @brief   Locks a mutex, blocking.
 *
 * @param[in,out]   mutex   Mutex object to lock.
 *
 * @pre     @p mutex is not `NULL`
 * @pre     Mutex at @p mutex has been initialized
 * @pre     Must be called in thread context
 *
 * @post    The mutex @p is locked and held by the calling thread.
 */
void mutex_lock(mutex_t *mutex);

/**
 * @brief   Locks a mutex, blocking. This function can be canceled.
 *
 * @param[in,out]   mc      Mutex cancellation structure to work on
 *
 * @retval  0               The mutex was locked by the caller
 * @retval  -ECANCELED      The mutex was ***NOT*** locked, operation was
 *                          canceled. See @ref mutex_cancel
 *
 * @note    This function is considered internal. Out of tree users should be
 *          aware that breaking API changes or removal of this API without
 *          an deprecation period might happen.
 *
 * @pre     Must be called in thread context
 * @pre     @p mc has been initialized with @ref mutex_cancel_init by the
 *          calling thread.
 * @pre     @p mc has ***NOT*** been used for previous calls to
 *          this function. (Reinitialize before reusing!)
 *
 * @post    The mutex referred to by @p mc is locked and held by the calling
 *          thread, unless `-ECANCELED` was returned.
 */
int mutex_lock_cancelable(mutex_cancel_t *mc);

/**
 * @brief   Unlocks the mutex.
 *
 * @param[in,out]   mutex   Mutex object to unlock.
 *
 * @pre     @p mutex is not `NULL`
 * @note    It is safe to unlock a mutex held by a different thread.
 * @note    It is safe to call this function from IRQ context.
 */
void mutex_unlock(mutex_t *mutex);

/**
 * @brief   Unlocks the mutex and sends the current thread to sleep
 *
 * @param[in,out]   mutex   Mutex object to unlock.
 * @pre     @p mutex is not `NULL`
 * @pre     Must be called in thread context.
 */
void mutex_unlock_and_sleep(mutex_t *mutex);

/**
 * @brief   Cancels a call to @ref mutex_lock_cancelable
 *
 * @param[in,out]   mc      Mutex cancellation structure referring to the
 *                          thread calling @ref mutex_lock_cancelable and to
 *                          the mutex to cancel the operation on
 *
 * @note    This function is considered internal. Out of tree users should be
 *          aware that breaking API changes or removal of this API without
 *          an deprecation period might happen.
 *
 * @pre     @p mc is used to cancel at most one call to
 *          @ref mutex_lock_cancelable. (You can reinitialize the same memory
 *          to safely reuse it.)
 * @warning You ***MUST NOT*** call this function once the thread referred to by
 *          @p mc re-uses the mutex object referred to by @p mc (not counting
 *          the call to @ref mutex_lock_cancelable @p mc was used in).
 * @note    It is safe to call this function from IRQ context, e.g. from a timer
 *          interrupt.
 * @note    It is safe to call this function more than once on the same @p mc
 *          while it is still valid (see the warning above). The first call will
 *          cancel the operation and subsequent calls will have no effect.
 *
 * @details If @p thread is currently running (or pending), a subsequent call
 *          from @p thread to @ref mutex_lock_cancelable will also fail
 *
 * Canonical use:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~{.c}
 * static void timeout_cb(void *arg) {
 *     mutex_cancel(arg);
 * }
 *
 * int ztimer_mutex_lock_timeout(ztimer_clock_t *clock, mutex_t *mutex,
 *                               uint32_t timeout)
 * {
 *     mutex_cancel_t mc = mutex_cancel_init(mutex);
 *     ztimer_t t = { .callback = timeout_cb, .arg = &mc };
 *     ztimer_set(clock, &t, timeout);
 *     if (mutex_lock_cancelable(&mc)) {
 *         return -ECANCELED;
 *     }
 *     ztimer_remove(clock, &t);
 *     return 0;
 * }
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * In the above example a simple implementation of how to implement mutex
 * locking with a timeout is given. There are two corner cases:
 *
 * 1. The call to @ref mutex_cancel could occur *before* the call to
 *    @ref mutex_lock_cancelable. (E.g. for `timeout == 0`.)
 * 2. The call to @ref mutex_cancel could occur right after the mutex was
 *    *successfully* obtained, but before `ztimer_remove()` was executed.
 *
 * In the first corner case the cancellation is stored in @p mc. Hence, the
 * subsequent call to @ref mutex_lock_cancelable gets indeed canceled. In the
 * second corner case the cancellation is also stored in @p mc but never used -
 * the mutex cancellation structure @p mc is not allowed to be reused without
 * reinitialization.
 */
void mutex_cancel(mutex_cancel_t *mc);

#ifdef __cplusplus
}
#endif

#endif /* MUTEX_H */
/** @} */
