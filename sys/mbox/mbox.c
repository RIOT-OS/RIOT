/*
 * Copyright (C) Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#include <assert.h>
#include <errno.h>
#include <inttypes.h>
#include <string.h>

#include "sched.h"

#include "mbox.h"

void mbox_create(mbox_t *mbox, msg_t *array, int num)
{
    assert((array != NULL) && (num <= (INT_MAX / 2)));
    mbox->msgs = array;                 /* Initialize all members */
    cib_init(&mbox->msgs_queue, num);
    mutex_init(&mbox->mutex);
    sema_create(&mbox->not_empty, 1);
    sema_create(&mbox->not_full, 0);
    mbox->waiting = 0;
}

void mbox_destroy(mbox_t *mbox)
{
    mutex_lock(&mbox->mutex);
    sema_destroy(&mbox->not_empty);     /* destroy semaphore to signal waiting threads */
    sema_destroy(&mbox->not_full);
    mbox->msgs = NULL;
    mutex_unlock(&mbox->mutex);
}

static int _mbox_put(mbox_t *mbox, msg_t *msg, int idx)
{
    msg_t *dst = &mbox->msgs[idx];

    msg->sender_pid = sched_active_pid;         /* set current thread as sender */
    *dst = *msg;                                /* copy message into queue */
    if (cib_avail(&mbox->msgs_queue) == 1) {    /* when queue was empty before */
        return sema_post(&mbox->not_empty);     /* signal to waiting threads */
    }
    return 0;
}

int mbox_put(mbox_t *mbox, msg_t *msg)
{
    int idx, res;

    assert((mbox != NULL) && (msg != NULL));
    mutex_lock(&mbox->mutex);
    if (mbox->msgs == NULL) {                       /* if no message queue => mbox was destroyed */
        mutex_unlock(&mbox->mutex);
        return -ECANCELED;
    }
    while ((idx = cib_put(&mbox->msgs_queue)) < 0) {    /* get next free index in message queue */
        mbox->waiting++;                                /* no free index in queue */
        mutex_unlock(&mbox->mutex);
        res = sema_wait(&mbox->not_full);               /* => wait for mbox not to be full */
        if (res < 0) {
            return res;
        }
        mutex_lock(&mbox->mutex);
        mbox->waiting--;
    }
    res = _mbox_put(mbox, msg, idx);
    mutex_unlock(&mbox->mutex);
    return res;
}

int mbox_tryput(mbox_t *mbox, msg_t *msg)
{
    int idx, res;

    assert((mbox != NULL) && (msg != NULL));
    mutex_lock(&mbox->mutex);
    if (mbox->msgs == NULL) {                       /* if no message queue => mbox was destroyed */
        mutex_unlock(&mbox->mutex);
        return -ECANCELED;
    }
    if ((idx = cib_put(&mbox->msgs_queue)) < 0) {   /* try to get free index in message queue */
        mutex_unlock(&mbox->mutex);                 /* no free index in queue */
        return -EAGAIN;                             /* return with error code */
    }
    res = _mbox_put(mbox, msg, idx);
    mutex_unlock(&mbox->mutex);
    return res;
}

static int _mbox_get(mbox_t *mbox, msg_t *msg)
{
    unsigned int idx = cib_get(&mbox->msgs_queue);  /* get next entry in queue */

    *msg = mbox->msgs[idx];
    if (mbox->waiting) {                            /* if there are messages waiting to be added
                                                     * to queue */
        return sema_post(&mbox->not_full);          /* signal their putters that queue isn't full
                                                     * anymore */
    }
    return 0;
}

int mbox_get_timed(mbox_t *mbox, msg_t *msg, uint64_t timeout)
{
    int res;

    assert((mbox != NULL) && (msg != NULL));
    mutex_lock(&mbox->mutex);
    if (mbox->msgs == NULL) {                       /* if no message queue => mbox was destroyed */
        mutex_unlock(&mbox->mutex);
        return -ECANCELED;
    }
    /* semaphore might not lock immediately, due to tryget not
     * locking and thus not cleaning up semaphore, so we loop
     * over semaphore lock, until we hit it */
    while (cib_avail(&mbox->msgs_queue) == 0) { /* wait until message queue isn't empty */
        mutex_unlock(&mbox->mutex);
        /* timeout == 0 => no timeout */
        res = sema_wait_timed(&mbox->not_empty, timeout);
        if (res < 0) {
            return res;
        }
        mutex_lock(&mbox->mutex);
    }
    res = _mbox_get(mbox, msg);
    mutex_unlock(&mbox->mutex);
    return res;
}

int mbox_tryget(mbox_t *mbox, msg_t *msg)
{
    int res;

    assert((mbox != NULL) && (msg != NULL));
    mutex_lock(&mbox->mutex);
    if (mbox->msgs == NULL) {                       /* if no message queue => mbox was destroyed */
        mutex_unlock(&mbox->mutex);
        return -ECANCELED;
    }
    if (cib_avail(&mbox->msgs_queue) == 0) {    /* no message in queue */
        mutex_unlock(&mbox->mutex);
        return -EAGAIN;                         /* => don't block */
    }
    res = _mbox_get(mbox, msg);
    mutex_unlock(&mbox->mutex);
    return res;
}

/** @} */
