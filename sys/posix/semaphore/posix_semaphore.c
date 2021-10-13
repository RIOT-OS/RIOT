/*
 * Copyright (C) 2017 TriaGnoSys GmbH
 *               2013 Freie Universität Berlin
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
 * @author  Víctor Ariño <victor.arino@zii.aero>
 */

#include <errno.h>
#include <inttypes.h>

#include "irq.h"
#include "sched.h"
#include "sema.h"
#include "timex.h"
#include "thread.h"
#include "xtimer.h"

#define ENABLE_DEBUG 0
#include "debug.h"

#include "semaphore.h"

int sem_timedwait(sem_t *sem, const struct timespec *abstime)
{
    uint64_t timeout = (((uint64_t)abstime->tv_sec) * US_PER_SEC) +
                       (abstime->tv_nsec / NS_PER_US);
    uint64_t now = xtimer_now_usec64();

    if (now > timeout) {
        errno = ETIMEDOUT;
        return -1;
    }
    timeout = timeout - now;
    int res = sema_wait_timed((sema_t *)sem, timeout);
    if (res < 0) {
        errno = -res;
        return -1;
    }
    return 0;
}

/** @} */
