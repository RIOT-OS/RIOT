/*
 * Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     isr_pipe_read_timeout
 * @{
 * @file
 * @brief       ISR -> userspace pipe with timeout implementation
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <errno.h>

#include "isrpipe/read_timeout.h"
#include "ztimer.h"

typedef struct {
    mutex_t *mutex;
    int flag;
} _isrpipe_timeout_t;

static void _cb(void *arg)
{
    _isrpipe_timeout_t *_timeout = (_isrpipe_timeout_t *) arg;

    _timeout->flag = 1;
    mutex_unlock(_timeout->mutex);
}

int isrpipe_read_timeout(isrpipe_t *isrpipe, uint8_t *buffer, size_t count, uint32_t timeout)
{
    int res = tsrb_get(&isrpipe->tsrb, buffer, count);
    if (res > 0) {
        return res;
    }

    _isrpipe_timeout_t _timeout = { .mutex = &isrpipe->mutex, .flag = 0 };
    ztimer_t timer = { .callback = _cb, .arg = &_timeout };

    ztimer_set(ZTIMER_USEC, &timer, timeout);
    while ((res = tsrb_get(&isrpipe->tsrb, buffer, count)) == 0) {
        mutex_lock(&isrpipe->mutex);
        if (_timeout.flag) {
            /* timer was consumed */
            return -ETIMEDOUT;
        }
    }

    ztimer_remove(ZTIMER_USEC, &timer);
    return res;
}

int isrpipe_read_all_timeout(isrpipe_t *isrpipe, uint8_t *buffer, size_t count, uint32_t timeout)
{
    uint8_t *pos = buffer;

    while (count) {
        int res = isrpipe_read_timeout(isrpipe, pos, count, timeout);
        if (res >= 0) {
            count -= res;
            pos += res;
        }
        else {
            return res;
        }
    }

    return pos - buffer;
}
