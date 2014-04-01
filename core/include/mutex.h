/*
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    core_sync Synchronization
 * @brief       Mutex for thread synchronization
 * @ingroup     core
 * @{
 *
 * @file        mutex.h
 * @brief       RIOT synchronization API
 *
 * @author      Freie Universität Berlin, Computer Systems & Telematics
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef __MUTEX_H_
#define __MUTEX_H_

#include "queue.h"

/**
 * @brief Mutex structure. Must never be modified by the user.
 */
typedef struct mutex_t {
    /* fields are managed by mutex functions, don't touch */
    /**
     * @internal
     * @brief   The value of the mutex; 0 if unlocked, 1 if locked. **Must
     *          never be changed by the user.**
     */
    unsigned int val;
    /**
     * @internal
     * @brief   The process waiting queue of the mutex. **Must never be changed
     *          by the user.**
     */
    queue_node_t queue;
} mutex_t;

/**
 * @brief Initializes a mutex object.
 *
 * @param[out] mutex    pre-allocated mutex structure, must not be NULL.
 *
 * @return  Always returns 1, always succeeds.
 */
int mutex_init(struct mutex_t *mutex);


/**
 * @brief Tries to get a mutex, non-blocking.
 *
 * @param[in] mutex Mutex object to lock. Has to be initialized first. Must not
 *                  be NULL.
 *
 * @return 1 if mutex was unlocked, now it is locked.
 * @return 0 if the mutex was locked.
 */
int mutex_trylock(struct mutex_t *mutex);

/**
 * @brief Tries to get a mutex, blocking.
 *
 * @param[in] mutex Mutex object to lock. Has to be initialized first. Must not
 *                  be NULL.
 *
 * @return 1 getting the mutex was successful
 * @return <1 there was an error.
 */
int mutex_lock(struct mutex_t *mutex);

/**
 * @brief Unlocks the mutex.
 *
 * @param[in] mutex Mutex object to unlock, must not be NULL.
 */
void mutex_unlock(struct mutex_t *mutex);

/**
 * @brief Unlocks the mutex and sends the current thread to sleep
 *
 * @param[in] mutex Mutex object to unlock, must not be NULL.
 */
void mutex_unlock_and_sleep(struct mutex_t *mutex);

#endif /* __MUTEX_H_ */
/** @} */
