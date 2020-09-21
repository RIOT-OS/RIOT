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

#include "irq.h"
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
} mutex_t;

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
    unsigned irq_state = irq_disable();
    int retval = 0;
    if (mutex->queue.next == NULL) {
        mutex->queue.next = MUTEX_LOCKED;
        retval = 1;
    };
    irq_restore(irq_state);
    return retval;
}

/**
 * @brief   Locks a mutex, blocking.
 *
 * @param[in,out]   mutex   Mutex object to lock.
 *
 * @retval  0               The mutex was locked by the caller
 * @retval  -ECANCELED      The mutex was ***NOT*** locked, operation was
 *                          canceled. See @ref mutex_cancel
 *
 * @pre     @p mutex is not `NULL`
 * @pre     Mutex at @p mutex has been initialized
 * @pre     Must be called in thread context
 *
 * @post    The mutex @p is locked and held by the calling thread, unless
 *          `-ECANCELED` was returned. This can only occur when
 *          @ref mutex_cancel is called for the given mutex and the calling
 *          thread.
 */
int mutex_lock(mutex_t *mutex);

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
 * @brief   If @p thread is currently blocked waiting for @p mutex, it will
 *          be unblocked without obtaining the mutex
 *
 * @param[in,out]   mutex   Mutex to perform the cancel action on
 * @param[in,out]   thread  Thread to remove from the @p mutex 's wait queue
 *
 * @note    This function is only provided when module `core_mutex_cancel` is
 *          used.
 * @note    It is safe to call this function from IRQ context, e.g. from a timer
 *          interrupt.
 *
 * @details If @p thread is currently running (or pending), a subsequent call
 *          from @p thread to @ref mutex_lock will also be canceled if @p mutex
 *          is passed as parameter. This avoids race conditions when @ref
 *          mutex_cancel get called just before @ref mutex_lock is called.
 */
void mutex_cancel(mutex_t *mutex, thread_t *thread);

#ifdef __cplusplus
}
#endif

#endif /* MUTEX_H */
/** @} */
