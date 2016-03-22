/*
 * Copyright (C) 2016 Engineering-Spirit
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     sys_newlib
 * @{
 *
 * @file
 * @brief       Newlib thread-safe recursive implementation
 *
 * @author      Nick v. IJzendoorn <nijzendoorn@engineering-spirit.nl>
 *
 * @}
 */

#include <assert.h>

#include "arch/irq_arch.h"
#include "newlib_lock.h"

void __recursive_init(recursive_mutex_t *rm)
{
    recursive_mutex_t empty_mutex = RECURSIVE_MUTEX_INIT;
    *rm = empty_mutex;
}

/**
 * @brief   __recursive_lock needs to provide recursive mutex locking
 */
void __recursive_lock(recursive_mutex_t *rm)
{
    /* TODO another way would be to avoid rescheduling other tasks */
#if USE_NEWLIB_THREAD_SAFE

    /* check if the mutex lock is not called from an interrupt */
    assert(!(irq_arch_in()));

    /* try to lock the recursive mutex */
    switch (mutex_trylock(&rm->mutex))
    {
    case 0:
        /* mutex is already locked */
        if (rm->owner != thread_getpid()) {
            /* we are not the owner, so we wait till it's released  and
             * fall-trough the case statement */
            mutex_lock(&rm->mutex);
        }

        /* fall-trough */

    case 1:
        /* mutex now locked by us */
        atomic_inc(&rm->recursion);
        rm->owner = thread_getpid();
        break;
    }
#else
    (void) rm;
#endif
}

void __recursive_unlock(recursive_mutex_t *rm)
{
#if USE_NEWLIB_THREAD_SAFE
    /* check if the mutex unlock is not called from an interrupt */
    assert(!(irq_arch_in()));

    /* check if the unlocking thread is the same as the locking thread */
    assert(rm->owner == thread_getpid());

    /* decrease the recursion counter */
    if (atomic_dec(&rm->recursion) == 1) {
        /* we just released the last recursion lock call */

        rm->owner = KERNEL_PID_UNDEF;
        mutex_unlock(&rm->mutex);
    }
#else
    (void) rm;
#endif
}
