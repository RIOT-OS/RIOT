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
 * @brief   Spin locks.
 * @note    Do not include this header file directly, but pthread.h.
 * @warning Spinlocks should be avoided.
 *          They will burn away the battery needlessly, and may not work because RIOT is tickless.
 *          Use irq_disable() and irq_restore() for shortterm locks instead.
 */

#ifndef PTHREAD_SPIN_H
#define PTHREAD_SPIN_H

#ifdef __cplusplus
#include <atomic>
using std::atomic_flag;
#else
#include <stdatomic.h>
#endif

/**
 * @brief           A spinlock.
 * @warning         Spinlocks should be avoided.
 *                  They will burn away the battery needlessly, and may not work because RIOT is tickless.
 *                  Use irq_disable() and irq_restore() for shortterm locks instead.
 */
typedef struct {
    atomic_flag flag; /**< Current lock state */
} pthread_spinlock_t;

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief           Intializes a spinlock.
 * @warning         See the warning in pthread_spinlock_t.
 * @details         A zeroed out datum is initialized.
 * @param[in,out]   lock      Datum to initialize.
 * @param[in]       pshared   Unused.
 * @returns         `0` on success.
 *                  `EINVAL` if `lock == NULL`.
 */
int pthread_spin_init(pthread_spinlock_t *lock, int pshared);

/**
 * @brief           Destroys a spinlock.
 * @warning         See the warning in pthread_spinlock_t.
 * @details         Destroying a spinlock while a thread is waiting for it causes undefined behavior.
 *                  This is a no-op.
 * @param[in,out]   lock   Datum to destroy.
 * @returns         `0` on success.
 *                  `EINVAL` if `lock == NULL`.
 */
int pthread_spin_destroy(pthread_spinlock_t *lock);

/**
 * @brief           Lock a spinlock.
 * @warning         See the warning in pthread_spinlock_t.
 * @param[in,out]   lock   Lock to acquire.
 * @return          `0` on success.
 *                  `EINVAL` if `lock == NULL`.
 */
int pthread_spin_lock(pthread_spinlock_t *lock);

/**
 * @brief           Tries to lock a spinlock, returns immediately if already locked.
 * @warning         See the warning in pthread_spinlock_t.
 * @param[in,out]   lock   Lock to acquire.
 * @return          `0` on success.
 *                  `EBUSY` if the lock was already locked.
 *                  `EINVAL` if `lock == NULL`.
 */
int pthread_spin_trylock(pthread_spinlock_t *lock);

/**
 * @brief           Releases a spinlock.
 * @warning         See the warning in pthread_spinlock_t.
 * @param[in,out]   lock   Lock to release
 * @return          `0` on success.
 *                  `EPERM` if the lock was not locked.
 *                  `EINVAL` if `lock == NULL`.
 */
int pthread_spin_unlock(pthread_spinlock_t *lock);

#ifdef __cplusplus
}
#endif

#endif /* PTHREAD_SPIN_H */

/**
 * @}
 */
