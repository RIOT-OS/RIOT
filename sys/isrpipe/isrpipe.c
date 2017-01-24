/*
 * Copyright (C) 2016 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup sys
 * @{
 * @file
 * @brief       ISR -> userspace pipe implementation
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 *
 * @}
 */

#include <errno.h>

#include "isrpipe.h"
#include "xtimer.h"

void isrpipe_init(isrpipe_t *isrpipe, char *buf, size_t bufsize)
{
    mutex_init(&isrpipe->mutex);
    tsrb_init(&isrpipe->tsrb, buf, bufsize);
}

int isrpipe_write_one(isrpipe_t *isrpipe, char c)
{
    int res = tsrb_add_one(&isrpipe->tsrb, c);

    /* `res` is either 0 on success or -1 when the buffer is full. Either way,
     * unlocking the mutex is fine.
     */
    mutex_unlock(&isrpipe->mutex);

    return res;
}

int isrpipe_read(isrpipe_t *isrpipe, char *buffer, size_t count)
{
    int res;

    while (!(res = tsrb_get(&isrpipe->tsrb, buffer, count))) {
        mutex_lock(&isrpipe->mutex);
    }
    return res;
}

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

int isrpipe_read_timeout(isrpipe_t *isrpipe, char *buffer, size_t count, uint32_t timeout)
{
    int res;

    _isrpipe_timeout_t _timeout = { .mutex = &isrpipe->mutex, .flag = 0 };

    xtimer_t timer = { .callback = _cb, .arg = &_timeout };

    xtimer_set(&timer, timeout);
    while (!(res = tsrb_get(&isrpipe->tsrb, buffer, count))) {
        mutex_lock(&isrpipe->mutex);
        if (_timeout.flag) {
            res = -ETIMEDOUT;
            break;
        }
    }

    xtimer_remove(&timer);
    return res;
}


int isrpipe_read_all_timeout(isrpipe_t *isrpipe, char *buffer, size_t count, uint32_t timeout)
{
    char *pos = buffer;

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
