/*
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 *
 * @author  Christian Mehlis <mehlis@inf.fu-berlin.de>
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 * @author  René Kijewski <kijewski@inf.fu-berlin.de>
 */

#include <errno.h>
#include <inttypes.h>

#include "irq.h"
#include "sched.h"
#include "sema.h"
#include "timex.h"
#include "thread.h"
#include "xtimer.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#include "semaphore.h"

int sem_timedwait(sem_t *sem, const struct timespec *abstime)
{
    uint64_t now, timeout = (((uint64_t)abstime->tv_sec) * SEC_IN_USEC) +
                            (abstime->tv_nsec / USEC_IN_NS);
    int res;
    now = xtimer_now64();
    if (now > timeout) {
        errno = ETIMEDOUT;
        return -1;
    }
    timeout = timeout - now;
    res = sema_wait_timed((sema_t *)sem, timeout);
    if (res < 0) {
        errno = -res;
        return -1;
    }
    return 0;
}

int sem_trywait(sem_t *sem)
{
    unsigned int old_state, value;
    int result;
    if (sem == NULL) {
        errno = EINVAL;
        return -1;
    }
    old_state = irq_disable();
    value = sem->value;
    if (value == 0) {
        errno = EAGAIN;
        result = -1;
    }
    else {
        result = 0;
        sem->value = value - 1;
    }

    irq_restore(old_state);
    return result;
}

/** @} */
