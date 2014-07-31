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
    *lock = 0;
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

    while (atomic_set_return((unsigned *) lock, 1) != 0) {
        /* spin */
    }
    return 0;
}

int pthread_spin_trylock(pthread_spinlock_t *lock)
{
    if (lock == NULL) {
        return EINVAL;
    }

    return atomic_set_return((unsigned *) lock, 1) == 0 ? 0 : EBUSY;
}

int pthread_spin_unlock(pthread_spinlock_t *lock)
{
    if (lock == NULL) {
        return EINVAL;
    }

    return atomic_set_return((unsigned *) lock, 0) != 0 ? 0 : EPERM;
}
