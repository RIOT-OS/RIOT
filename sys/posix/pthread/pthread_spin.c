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
 * @author  Joakim Nohlgård <joakim.nohlgard@eistec.se>
 * @}
 */

#include <stdint.h>
#include <stdatomic.h>
#include <errno.h>
#include "pthread.h"

int pthread_spin_init(pthread_spinlock_t *lock, int pshared)
{
    if (lock == NULL) {
        return EINVAL;
    }

    (void) pshared;
    atomic_flag_clear(&(lock->flag));
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

    while (atomic_flag_test_and_set(&(lock->flag))) {
        /* spin */
    }

    return 0;
}

int pthread_spin_trylock(pthread_spinlock_t *lock)
{
    if (lock == NULL) {
        return EINVAL;
    }

    if (atomic_flag_test_and_set(&(lock->flag))) {
        return EBUSY;
    }

    return 0;
}

int pthread_spin_unlock(pthread_spinlock_t *lock)
{
    if (lock == NULL) {
        return EINVAL;
    }

    atomic_flag_clear(&(lock->flag));

    return 0;
}
