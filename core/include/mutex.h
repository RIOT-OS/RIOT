/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *               2013, 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    core_sync Synchronization
 * @brief       Mutex for thread synchronization
 * @ingroup     core
 * @{
 *
 * @file
 * @brief       RIOT synchronization API
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef MUTEX_H
#define MUTEX_H

#include <stddef.h>

#include "list.h"

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
 * @brief Initializes a mutex object.
 * @details For initialization of variables use MUTEX_INIT instead.
 *          Only use the function call for dynamically allocated mutexes.
 * @param[out] mutex    pre-allocated mutex structure, must not be NULL.
 */
static inline void mutex_init(mutex_t *mutex)
{
    mutex->queue.next = NULL;
}

/**
 * @brief Lock a mutex, blocking or non-blocking.
 *
 * @details For commit purposes you should probably use mutex_trylock() and
 *          mutex_lock() instead.
 *
 * @param[in] mutex         Mutex object to lock. Has to be initialized first.
 *                          Must not be NULL.
 * @param[in] blocking      if true, block until mutex is available.
 *
 * @return 1 if mutex was unlocked, now it is locked.
 * @return 0 if the mutex was locked.
 */
int _mutex_lock(mutex_t *mutex, int blocking);

/**
 * @brief Tries to get a mutex, non-blocking.
 *
 * @param[in] mutex Mutex object to lock. Has to be initialized first. Must not
 *                  be NULL.
 *
 * @return 1 if mutex was unlocked, now it is locked.
 * @return 0 if the mutex was locked.
 */
static inline int mutex_trylock(mutex_t *mutex)
{
    return _mutex_lock(mutex, 0);
}

/**
 * @brief Locks a mutex, blocking.
 *
 * Mutexes are served in FIFO order while respecting thread priority (i.e. a higher
 * priority thread will be unlocked before a lower priority one even if it arrived
 * later). As long as no thread holds the mutex indefinitely and as long as there
 * are no deadlocks, all same-priority threads blocked on mutex_lock() should
 * eventually unblock.
 *
 * @param[in] mutex Mutex object to lock. Has to be initialized first. Must not be NULL.
 */
static inline void mutex_lock(mutex_t *mutex)
{
    _mutex_lock(mutex, 1);
}

/**
 * @brief Unlocks the mutex.
 *
 * One of the threads waiting on the mutex will be unblocked. For details on the
 * order in which mutexes are served see mutex_lock().
 *
 * A mutex in RIOT can be unlocked by any thread and not just the one that is
 * holding the mutex. This can be used to construct other synchronization devices.
 *
 * @param[in] mutex Mutex object to unlock, must not be NULL.
 */
void mutex_unlock(mutex_t *mutex);

/**
 * @brief Unlocks the mutex and sends the current thread to sleep
 *
 * @param[in] mutex Mutex object to unlock, must not be NULL.
 */
void mutex_unlock_and_sleep(mutex_t *mutex);

#ifdef __cplusplus
}
#endif

#endif /* MUTEX_H */
/** @} */
