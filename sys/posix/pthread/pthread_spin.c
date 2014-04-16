/*
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file subject to the terms and conditions of the GNU Lesser General
 * Public License. See the file LICENSE in the top level directory for more
 * details.
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

int pthread_spin_init(pthread_spinlock_t *lock, int pshared)
{
    (void) pshared;
    *lock = 0;
    return 0;
}

int pthread_spin_destroy(pthread_spinlock_t *lock)
{
    (void) lock;
    return 0;
}

int pthread_spin_lock(pthread_spinlock_t *lock)
{
    while (*lock) {
        ; /* spin */
    }
    return 0;
}

int pthread_spin_trylock(pthread_spinlock_t *lock)
{
    if (*lock) {
        return 0;
    }

    return -1;
}

int pthread_spin_unlock(pthread_spinlock_t *lock)
{
    --*lock;
    return 0;
}
