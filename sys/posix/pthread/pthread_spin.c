/*
 * Copyright (C) 2013 Freie Universität Berlin
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
 * @author  Christian Mehlis <mehlis@inf.fu-berlin.de>
 * @author  René Kijewski <kijewski@inf.fu-berlin.de>
 * @author  Joakim Gebart <joakim.gebart@eistec.se>
 * @}
 */

#include "pthread.h"
#include "atomic.h"

int pthread_spin_init(pthread_spinlock_t *lock, int pshared)
{
    if (lock == NULL) {
        return EINVAL;
    }

    (void) pshared;
    ATOMIC_VALUE(lock->value) = 0;
    return 0;
}

int pthread_spin_destroy(pthread_spinlock_t *lock)
{
    if (lock == NULL) {
        return EINVAL;
    }

    (void) lock;
    return 0;
}

int pthread_spin_lock(pthread_spinlock_t *lock)
{
    if (lock == NULL) {
        return EINVAL;
    }

    while (atomic_set_to_one(&(lock->value)) == 0) {
        /* spin */
    }

    return 0;
}

int pthread_spin_trylock(pthread_spinlock_t *lock)
{
    if (lock == NULL) {
        return EINVAL;
    }

    if (atomic_set_to_one(&(lock->value)) == 0) {
        return EBUSY;
    }

    return 0;
}

int pthread_spin_unlock(pthread_spinlock_t *lock)
{
    if (lock == NULL) {
        return EINVAL;
    }

    if (atomic_set_to_zero(&(lock->value)) == 0) {
        return EPERM;
    }

    return 0;
}
