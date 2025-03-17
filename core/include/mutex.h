/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *               2013, 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @addtogroup     core_sync_mutex
 * @{
 */
/**
 * @file
 * @brief       Mutex for thread synchronization
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef MUTEX_H
#define MUTEX_H

#include <stddef.h>
#include <stdint.h>
#include <stdbool.h>

#include "architecture.h"
#include "kernel_defines.h"
#include "list.h"
#include "thread.h"

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
#if defined(DOXYGEN) || defined(MODULE_CORE_MUTEX_PRIORITY_INHERITANCE) \
    || defined(MODULE_CORE_MUTEX_DEBUG)
    /**
     * @brief   The current owner of the mutex or `NULL`
     * @note    Only available if module core_mutex_priority_inheritance
     *          is used.
     *
     * If either the mutex is not locked or the mutex is not locked by a thread
     * (e.g. because it is used to synchronize a thread with an ISR completion),
     * this will have the value of `NULL`.
     */
    kernel_pid_t owner;
#endif
#if defined(DOXYGEN) || defined(MODULE_CORE_MUTEX_DEBUG)
    /**
     * @brief   Program counter of the call to @ref mutex_lock that most
     *          recently acquired this mutex
     *
     * This is used when the module `core_mutex_debug` is used to debug
     * deadlocks and is non-existing otherwise
     */
    uinttxtptr_t owner_calling_pc;
#endif
#if defined(DOXYGEN) || defined(MODULE_CORE_MUTEX_PRIORITY_INHERITANCE)
    /**
     * @brief   Original priority of the owner
     * @note    Only available if module core_mutex_priority_inheritance
     *          is used.
     */
    uint8_t owner_original_priority;
#endif
} mutex_t;

/**
 * @brief   Internal function implementing @ref mutex_lock and
 *          @ref mutex_trylock
 *
 * @details Do not call this function, use @ref mutex_lock or @ref mutex_trylock
 *          instead
 *
 * @param[in,out]   mutex   Mutex object to lock.
 * @param[in]       block   Whether to block
 *
 * @pre     @p mutex is not `NULL`
 * @pre     Mutex at @p mutex has been initialized
 * @pre     Must be called in thread context
 *
 * @post    The mutex @p is locked and held by the calling thread.
 *
 * @retval  true    Mutex obtained
 * @retval  false   Mutex not obtained (only possible if @p block is `false`)
 */
bool mutex_lock_internal(mutex_t *mutex, bool block);

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

#ifndef __cplusplus
/**
 * @brief Static initializer for mutex_t.
 * @details This initializer is preferable to mutex_init().
 */
#  define MUTEX_INIT { .queue = { .next = NULL } }

/**
 * @brief Static initializer for mutex_t with a locked mutex
 */
#  define MUTEX_INIT_LOCKED { .queue = { .next = MUTEX_LOCKED } }
#else
#  define MUTEX_INIT {}
#  define MUTEX_INIT_LOCKED { { MUTEX_LOCKED } }
#endif /* __cplusplus */

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
 * @brief   Initializes a mutex object in a locked state.
 * @details For initialization of variables use MUTEX_INIT_LOCKED instead.
 *          Only use the function call for dynamically allocated mutexes.
 * @param[out]      mutex   pre-allocated mutex structure, must not be NULL.
 */
static inline void mutex_init_locked(mutex_t *mutex)
{
    *mutex = (mutex_t)MUTEX_INIT_LOCKED;
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
    return mutex_lock_internal(mutex, false);
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
static inline void mutex_lock(mutex_t *mutex)
{
#if (MAXTHREADS > 1)
    mutex_lock_internal(mutex, true);
#else
    /* dummy implementation for when no scheduler is used */
    /* (ab)use next pointer as lock variable */
    volatile uintptr_t *lock = (void *)&mutex->queue.next;

    /* spin until lock is released (e.g. by interrupt).
     *
     * Note: since only the numbers 0 and 1 are ever stored in lock, this
     * read does not need to be atomic here - even while a concurrent write
     * is performed on lock, a read will still either yield 0 or 1 (so the old
     * or new value, which both is fine), even if the lock is read out byte-wise
     * (e.g. on AVR).
     */
    while (*lock) {}

    /* set lock variable */
    *lock = 1;
#endif
}

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
#if (MAXTHREADS > 1) || DOXYGEN
void mutex_unlock(mutex_t *mutex);
#else
/**
 * @brief   dummy implementation for when no scheduler is used
 */
static inline void mutex_unlock(mutex_t *mutex)
{
    /* (ab)use next pointer as lock variable */
    mutex->queue.next = NULL;
}
#endif

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
 *          @p mc reuses the mutex object referred to by @p mc (not counting
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
