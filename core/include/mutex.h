/*
 * Copyright (C) 2013, 2014 Freie Universität Berlin
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

#ifndef MUTEX_H_
#define MUTEX_H_

#include "priority_queue.h"

#ifdef __cplusplus
 extern "C" {
#endif

/**
 * @brief Mutex structure. Must never be modified by the user.
 */
typedef struct mutex_t {
    /* fields are managed by mutex functions, don't touch */
    /**
     * @brief   The value of the mutex; 0 if unlocked, 1 if locked. **Must
     *          never be changed by the user.**
     * @internal
     */
    unsigned int val;
    /**
     * @brief   The process waiting queue of the mutex. **Must never be changed
     *          by the user.**
     * @internal
     */
    priority_queue_t queue;
} mutex_t;

/**
 * @brief Static initializer for mutex_t.
 * @details This initializer is preferable to mutex_init().
 */
#define MUTEX_INIT { 0, PRIORITY_QUEUE_INIT }

/**
 * @brief Initializes a mutex object.
 * @details For initialization of variables use MUTEX_INIT instead.
 *          Only use the function call for dynamically allocated mutexes.
 * @param[out] mutex    pre-allocated mutex structure, must not be NULL.
 */
static inline void mutex_init(mutex_t *mutex)
{
    mutex_t empty_mutex = MUTEX_INIT;
    *mutex = empty_mutex;
}

/**
 * @brief Tries to get a mutex, non-blocking.
 *
 * @param[in] mutex Mutex object to lock. Has to be initialized first. Must not
 *                  be NULL.
 *
 * @return 1 if mutex was unlocked, now it is locked.
 * @return 0 if the mutex was locked.
 */
int mutex_trylock(mutex_t *mutex);

/**
 * @brief Locks a mutex, blocking.
 *
 * @param[in] mutex Mutex object to lock. Has to be initialized first. Must not be NULL.
 */
void mutex_lock(mutex_t *mutex);

/**
 * @brief Unlocks the mutex.
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

#endif /* MUTEX_H_ */
/** @} */
