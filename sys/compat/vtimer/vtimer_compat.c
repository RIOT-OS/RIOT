/*
 * Copyright (C) 2015 Kaspar Schleiser <kaspar@schleiser.de>
 *
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_compat
 * @{
 *
 * @file
 * @brief       Implementation of the vtimer interface over xtimers
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include "xtimer.h"
#include "timex.h"
#include "vtimer.h"

#include <time.h>
#include <sys/time.h>
#include <string.h>

int vtimer_sleep(timex_t time)
{
    xtimer_usleep64(timex_uint64(time));
    return 0;
}

int vtimer_msg_receive_timeout(msg_t *m, timex_t timeout)
{
    uint64_t _timeout = timex_uint64(timeout);
    return xtimer_msg_receive_timeout64(m, _timeout);
}

void vtimer_now(timex_t *out)
{
    *out = timex_from_uint64(xtimer_now64());
}

void vtimer_get_localtime(struct tm *localt)
{
    timex_t now;
    vtimer_now(&now);

    memset(localt, 0, sizeof(struct tm));
    localt->tm_sec = now.seconds % 60;
    localt->tm_min = (now.seconds / 60) % 60;
    localt->tm_hour = (now.seconds / 60 / 60) % 24;
}

void vtimer_set_msg(vtimer_t *t, timex_t interval, kernel_pid_t pid, uint16_t type, void *ptr)
{
    t->msg.type = type;
    t->msg.content.ptr = ptr;

    xtimer_set_msg64(&t->timer, timex_uint64(interval), &t->msg, pid);
}

int vtimer_set_wakeup(vtimer_t *t, timex_t interval, kernel_pid_t pid)
{
    xtimer_set_wakeup64(&t->timer, timex_uint64(interval), pid);

    return 0;
}

void vtimer_remove(vtimer_t *t)
{
    xtimer_remove(&t->timer);
}

/*
 * This is a workaround for missing support in clang on OSX,
 * the alias is not needed in native.
 * Compare https://github.com/RIOT-OS/RIOT/issues/2336
 */
#ifndef BOARD_NATIVE
void _gettimeofday(void)           __attribute__ ((weak, alias("vtimer_gettimeofday")));
#endif

void vtimer_gettimeofday(struct timeval *tp)
{
    timex_t now;
    vtimer_now(&now);

    tp->tv_sec = now.seconds;
    tp->tv_usec = now.microseconds;
}
