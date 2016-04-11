/*
 * Copyright (C) 2014 René Kijewski <rene.kijewski@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup pthread
 * @{
 * @file
 * @brief   Implementation of a fair, POSIX conforming reader/writer lock.
 * @note    Do not include this header file directly, but pthread.h.
 */

#ifndef __SYS__POSIX__PTHREAD_RWLOCK__H
#define __SYS__POSIX__PTHREAD_RWLOCK__H

#include "priority_queue.h"
#include "thread.h"

#include <errno.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief     A fair reader writer lock.
 * @details   The implementation ensures that readers and writers of the same priority
 *            won't starve each other.
 *            E.g. no new readers will get into the critical section
 *            if a writer of the same or a higher priority already waits for the lock.
 */
typedef struct
{
    /**
     * @brief     The current amount of reader inside the critical section.
     * @details
     *            * `== 0`: no thread is in the critical section.
     *            * `> 0`: the number of readers currently in the critical section.
     *            * `< 0`: a writer is currently in the critical section.
     */
    int readers;

    /**
     * @brief     Queue of waiting threads.
     */
    priority_queue_t queue;

    /**
     * @brief     Provides mutual exclusion on reading and writing on the structure.
     */
    mutex_t mutex;
} pthread_rwlock_t;

/**
 * @brief     Internal structure that stores one waiting thread.
 */
typedef struct {
    bool is_writer;                 /**< `false`: reader; `true`: writer */
    thread_t *thread;               /**< waiting thread */
    priority_queue_node_t qnode;    /**< Node to store in `pthread_rwlock_t::queue`. */
    bool continue_;                 /**< This is not a spurious wakeup. */
} __pthread_rwlock_waiter_node_t;

/**
 * @brief           Initialize a reader/writer lock.
 * @details         A zeroed out datum is initialized.
 * @param[in,out]   rwlock   Lock to initialize.
 * @param[in]       attr     Unused.
 * @returns         `0` on success.
 *                  `EINVAL` if `rwlock == NULL`.
 */
int pthread_rwlock_init(pthread_rwlock_t *rwlock, const pthread_rwlockattr_t *attr);

/**
 * @brief           Destroy a reader/writer lock.
 * @details         This is a no-op.
 *                  Destroying a reader/writer lock while a thread holds it, or
 *                  there are threads waiting for it causes undefined behavior.
 *                  Datum must be reinitialized before using it again.
 * @param[in]       rwlock   Lock to destroy.
 * @returns         `0` on success.
 *                  `EINVAL` if `rwlock == NULL`.
 *                  `EBUSY` if the lock was in use.
 */
int pthread_rwlock_destroy(pthread_rwlock_t *rwlock);

/**
 * @brief           Lock a reader/writer lock for reading.
 * @details         This function may block.
 * @param[in]       rwlock   Lock to acquire for reading.
 * @returns         `0` if the lock could be acquired.
 *                  `EINVAL` if `rwlock == NULL`.
 */
int pthread_rwlock_rdlock(pthread_rwlock_t *rwlock);

/**
 * @brief           Try to lock a reader/writer lock for reader.
 * @details         This function won't block.
 * @param[in]       rwlock   Lock to acquire for reading.
 * @returns         `0` if the lock could be acquired.
 *                  `EBUSY` if acquiring the lock cannot be done without blocking.
 *                  `EINVAL` if `rwlock == NULL`.
 */
int pthread_rwlock_tryrdlock(pthread_rwlock_t *rwlock);

/**
 * @brief           Try to acquire a read lock in a given timeframe
 * @param[in]       rwlock    Lock to acquire for reading.
 * @param[in]       abstime   Maximum timestamp when to wakeup, absolute.
 * @returns         `0` if the lock could be acquired.
 *                  `EDEADLK` if the lock could not be acquired in the given timeframe.
 *                  `EINVAL` if `rwlock == NULL`.
 */
int pthread_rwlock_timedrdlock(pthread_rwlock_t *rwlock, const struct timespec *abstime);

/**
 * @brief           Lock a reader/writer lock for writing.
 * @details         This function may block.
 * @param[in]       rwlock   Lock to acquire for writing.
 * @returns         `0` if the lock could be acquired.
 *                  `EINVAL` if `rwlock == NULL`.
 */
int pthread_rwlock_wrlock(pthread_rwlock_t *rwlock);

/**
 * @brief           Try to lock a reader/writer lock for writing.
 * @details         This function won't block.
 * @param[in]       rwlock   Lock to acquire for writing.
 * @returns         `0` if the lock could be acquired.
 *                  `EBUSY` if acquiring the lock cannot be done without blocking.
 *                  `EINVAL` if `rwlock == NULL`.
 */
int pthread_rwlock_trywrlock(pthread_rwlock_t *rwlock);

/**
 * @brief           Try to acquire a write lock in a given timeframe
 * @param[in]       rwlock    Lock to acquire for writing.
 * @param[in]       abstime   Maximum timestamp when to wakeup, absolute.
 * @returns         `0` if the lock could be acquired.
 *                  `EDEADLK` if the lock could not be acquired in the given timeframe.
 *                  `EINVAL` if `rwlock == NULL`.
 */
int pthread_rwlock_timedwrlock(pthread_rwlock_t *rwlock, const struct timespec *abstime);

/**
 * @brief           Unlock the reader/writer lock.
 * @details         Must only be used if the lock is currently held.
 *                  You may release the lock out of another thread, but the *lock*, *operate*, *unlock* logic must not be broken.
 * @param[in]       rwlock   Lock to release
 * @returns         `0` on success.
 *                  `EPERM` if the lock was not held for any operation.
 *                  `EINVAL` if `rwlock == NULL`.
 */
int pthread_rwlock_unlock(pthread_rwlock_t *rwlock);

/**
 * @brief            Internal function to determine of the lock can be acquired for reading.
 * @param[in]        rwlock   Lock to query.
 * @returns          `false` if locking for reading is possible without blocking.
 */
bool __pthread_rwlock_blocked_readingly(const pthread_rwlock_t *rwlock);

/**
 * @brief            Internal function to determine of the lock can be acquired for writing.
 * @param[in]        rwlock   Lock to query.
 * @returns          `false` if locking for writing is possible without blocking.
 */
bool __pthread_rwlock_blocked_writingly(const pthread_rwlock_t *rwlock);

#ifdef __cplusplus
}
#endif

#endif

/**
 * @}
 */
