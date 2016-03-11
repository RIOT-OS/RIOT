/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup pthread
 * @{
 * @file
 * @brief   Mutual exclusion.
 * @note    Do not include this header file directly, but pthread.h.
 */

#ifndef __SYS__POSIX__PTHREAD_MUTEX__H
#define __SYS__POSIX__PTHREAD_MUTEX__H

#include <time.h>

#include "mutex.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief           Pthread mutexes are quite the same as RIOT mutexes.
 * @details         Recursive locking is not supported.
 *                  A thread can unlock a mutex even if it does not hold it.
 */
typedef mutex_t pthread_mutex_t;

/**
 * @brief           Initialize a mutex.
 * @details         A zeroed out datum is initialized.
 * @param[in,out]   mutex       Mutex to initialize.
 * @param[in]       mutexattr   Unused.
 * @returns         `0` on success. `-1` iff `mutex == NULL`.
 */
int pthread_mutex_init(pthread_mutex_t *mutex, const pthread_mutexattr_t *mutexattr);

/**
 * @brief           Destroy a mutex.
 * @details         This is currently a no-op.
 *                  Destroying a mutex locked is undefined behavior.
 * @param[in,out]   mutex   Datum to destroy.
 * @returns         0, this invocation is a no-op that cannot fail.
 */
int pthread_mutex_destroy(pthread_mutex_t *mutex);

/**
 * @brief           Try to lock a mutex.
 * @details         This function won't block.
 * @param[in]       mutex   Mutex to lock, must be initialized.
 * @returns         `0` if you hold the mutex now.
 *                  `+1` if the mutex already was locked.
 *                  `-1` if you managed to supply `NULL`.
 */
int pthread_mutex_trylock(pthread_mutex_t *mutex);

/**
 * @brief           Lock and hold a mutex.
 * @details         This invocation may block if the mutex was locked.
 * @param[in]       mutex   Mutex to lock, must be initialized.
 * @returns         `-1` iff you managed to supply `NULL`.
 *                  `0` otherwise, you hold the mutex now.
 */
int pthread_mutex_lock(pthread_mutex_t *mutex);

/**
 * @brief           Not implemented, yet.
 * @details         Will cause a linktime error ...
 * @param[in]       mutex     The unused mutex.
 * @param[in]       abstime   The used absolute time.
 * @return          Well ... you'll get a link time error, so nothing will be returned.
 */
int pthread_mutex_timedlock(pthread_mutex_t *mutex, const struct timespec *abstime);

/**
 * @brief           Unlock a mutex.
 * @details         It is possible to unlock a mutex that you don't hold.
 *                  It is possible to unlock a mutex that is not held at all.
 *                  The mutex can still be locked afterwards if there were threads queuing for this mutex.
 * @param[in]       mutex   Mutex to unlock, must be initialized.
 * @returns         `-1` iff you managed to supply `NULL`.
 *                  `0` otherwise.
 */
int pthread_mutex_unlock(pthread_mutex_t *mutex);

/**
 * @brief           Not implemented, yet.
 * @details         Will cause a linktime error ...
 * @param[in]       mutex         The unused mutex.
 * @param[out]      prioceiling   Unused.
 * @return          Well ... you'll get a link time error, so nothing will be returned.
 */
int pthread_mutex_getprioceiling(const pthread_mutex_t *mutex, int *prioceiling);

/**
 * @brief           Not implemented, yet.
 * @details         Will cause a linktime error ...
 * @param[in,out]   mutex         The unused mutex.
 * @param[in]       prioceiling   Unused.
 * @param[out]      old_ceiling   Unused.
 * @return          Well ... you'll get a link time error, so nothing will be returned.
 */
int pthread_mutex_setprioceiling(pthread_mutex_t *mutex, int prioceiling, int *old_ceiling);

#ifdef __cplusplus
}
#endif

#endif

/**
 * @}
 */
