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

#ifndef _MUTEX_H
#define _MUTEX_H

#include "queue.h"

/**
 * @brief Mutex structure. Should never be modified by the user.
 */
typedef struct mutex_t {
    /* fields are managed by mutex functions, don't touch */
    unsigned int val;       // @internal
    queue_node_t queue;     // @internal
} mutex_t;

/**
 * @brief Initializes a mutex object
 * @param mutex pre-allocated mutex structure.
 * @return Always returns 1, always succeeds.
 */
int mutex_init(struct mutex_t *mutex);


/**
 * @brief Tries to get a mutex, non-blocking.
 *
 * @param mutex Mutex-Object to lock. Has to be initialized first.
 *
 * @return 1 if mutex was unlocked, now it is locked.
 * @return 0 if the mutex was locked.
 */
int mutex_trylock(struct mutex_t *mutex);

/**
 * @brief Tries to get a mutex, blocking.
 *
 * @param mutex Mutex-Object to lock. Has to be initialized first.
 *
 * @return 1 getting the mutex was successful
 * @return <1 there was an error.
 */
int mutex_lock(struct mutex_t *mutex);

/**
 * @brief Unlocks the mutex.
 *
 * @param mutex Mutex-Object to unlock.
 */
void mutex_unlock(struct mutex_t *mutex);

/**
 * @brief Atomically unlocks the mutex and sends the current thread to sleep
 *
 * @param mutex Mutex-Object to unlock.
 */
void mutex_unlock_and_sleep(struct mutex_t *mutex);

/**
 * @brief Atomically unlock a mutex and lock another
 *
 * @param old_mutex Mutex to unlock
 * @param new_mutex Mutex to lock
 */
void mutex_unlock_and_lock(struct mutex_t *old_mutex, struct mutex_t *new_mutex);

/** @} */
#endif /* _MUTEX_H */
