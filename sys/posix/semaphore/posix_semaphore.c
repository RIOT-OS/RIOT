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
#include "sem.h"
#include "tcb.h"
#include "timex.h"
#include "thread.h"
#include "vtimer.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#include "semaphore.h"

#define USEC_IN_NS  (1000)

int sem_timedwait(sem_t *sem, const struct timespec *abstime)
{
    timex_t now, timeout = { abstime->tv_sec, abstime->tv_nsec / USEC_IN_NS };
    int res;
    vtimer_now(&now);
    if (timex_cmp(now, timeout) > 0) {
        errno = ETIMEDOUT;
        return -ETIMEDOUT;
    }
    timeout = timex_sub(timeout, now);
    res = sem_wait_timed(sem, &timeout);
    if (res < 0) {
        errno = -res;
    }
    return res;
}

int sem_trywait(sem_t *sem)
{
    unsigned int old_state, value;
    int result;
    if (sem == NULL) {
        errno = EINVAL;
        return -EINVAL;
    }
    old_state = disableIRQ();
    value = sem->value;
    if (value == 0) {
        errno = EAGAIN;
        result = -EAGAIN;
    }
    else {
        result = 0;
        sem->value = value - 1;
    }

    restoreIRQ(old_state);
    return result;
}

/** @} */
