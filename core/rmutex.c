/*
 * Copyright (C) 2016 Theobroma Systems Design & Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     core_sync
 * @{
 *
 * @file
 * @brief       RIOT synchronization API
 *
 * @author      Martin Elshuber <martin.elshuber@theobroma-systems.com>
 *
 * The recursive mutex implementation is inspired by the implementetaion of
 * Nick v. IJzendoorn <nijzendoorn@engineering-spirit.nl>
 * @see https://github.com/RIOT-OS/RIOT/pull/4529/files#diff-8f48e1b9ed7a0a48d0c686a87cc5084eR35
 *
 */

#include <stdio.h>
#include <inttypes.h>

#include "rmutex.h"
#include "thread.h"
#include "assert.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

static int _lock(rmutex_t *rmutex, int trylock)
{
    kernel_pid_t owner;

    /* try to lock the mutex */
    DEBUG("rmutex %" PRIi16" : trylock\n", thread_getpid());
    if (mutex_trylock(&rmutex->mutex) == 0) {
        DEBUG("rmutex %" PRIi16" : mutex already held\n", thread_getpid());
        /* Mutex is already held
         *
         * Case 1: Mutex is not held by me
         *     Condition 1: holds
         *     rmutex->owner != thread_getpid()
         *
         * Note for Case 1:
         *
         *     As a consequence it is necessaray to call
         *     mutex_lock(). However the read access to owner is not
         *     locked, and owner can be changed by a thread that is
         *     holding the lock (e.g.: holder unlocks the mutex, new
         *     holder acquired the lock). The atomic access strategy
         *     'relaxed' ensures, that the value of rmutex->owner is read
         *     consistent.
         *
         *     It is not necessary to synchronize (make written values
         *     visible) read/write with other threads, because every
         *     write by other threads let evaluate Condition 1 to
         *     false. They all write either KERNEL_PID_UNDEF or the
         *     pid of the other thread.
         *
         *     Other threads never set rmutex->owner to the pid of the
         *     current thread. Hence, it is guaranteed that mutex_lock
         *     is eventually called.
         *
         * Case 2: Mutex is held be me (relock)
         *     Condition 2: holds
         *     rmutex->owner == thread_getpid()
         *
         * Note for Case 2:
         *
         *     Because the mutex rmutex->owner is only written be the
         *     owner (me), rmutex->owner stays constant throughout the
         *     complete call and rmutex->refcount is protected
         *     (read/write) by the mutex.
         */

        /* ensure that owner is read atomically, since I need a consistent value */
        owner = atomic_load_explicit(&rmutex->owner, memory_order_relaxed);
        DEBUG("rmutex %" PRIi16" : mutex held by %" PRIi16" \n", thread_getpid(), owner);

        /* Case 1: Mutex is not held by me */
        if (owner != thread_getpid()) {
            /* wait for the mutex */
            DEBUG("rmutex %" PRIi16" : locking mutex\n", thread_getpid());

            if (trylock) {
                return 0;
            }
            else {
                mutex_lock(&rmutex->mutex);
            }
        }
        /* Case 2: Mutex is held be me (relock) */
        /* Note: There is nothing to do for Case 2; refcount is incremented below */
    }

    DEBUG("rmutex %" PRIi16" : I am now holding the mutex\n", thread_getpid());

    /* I am holding the recursive mutex */
    DEBUG("rmutex %" PRIi16" : setting the owner\n", thread_getpid());

    /* ensure that owner is written atomically, since others need a consistent value */
    atomic_store_explicit(&rmutex->owner, thread_getpid(), memory_order_relaxed);

    DEBUG("rmutex %" PRIi16" : increasing refs\n", thread_getpid());

    /* increase the refcount */
    rmutex->refcount++;

    return 1;
}

void rmutex_lock(rmutex_t *rmutex)
{
    _lock(rmutex, 0);
}

int rmutex_trylock(rmutex_t *rmutex)
{
    return _lock(rmutex, 1);
}

void rmutex_unlock(rmutex_t *rmutex)
{
    assert(atomic_load_explicit(&rmutex->owner,memory_order_relaxed) == thread_getpid());
    assert(rmutex->refcount > 0);

    DEBUG("rmutex %" PRIi16" : decrementing refs refs\n", thread_getpid());

    /* decrement refcount */
    rmutex->refcount--;

    /* check if I should still hold the mutex */
    if (rmutex->refcount == 0) {
        /* if not release the mutex */

        DEBUG("rmutex %" PRIi16" : resetting owner\n", thread_getpid());

        /* ensure that owner is written only once */
        atomic_store_explicit(&rmutex->owner, KERNEL_PID_UNDEF, memory_order_relaxed);

        DEBUG("rmutex %" PRIi16" : releasing mutex\n", thread_getpid());

        mutex_unlock(&rmutex->mutex);
    }
}
