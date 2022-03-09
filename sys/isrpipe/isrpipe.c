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

#include "isrpipe.h"

void isrpipe_init(isrpipe_t *isrpipe, uint8_t *buf, size_t bufsize)
{
    mutex_init(&isrpipe->mutex);
    tsrb_init(&isrpipe->tsrb, buf, bufsize);
}

int isrpipe_write_one(isrpipe_t *isrpipe, uint8_t c)
{
    int res = tsrb_add_one(&isrpipe->tsrb, c);

    /* `res` is either 0 on success or -1 when the buffer is full. Either way,
     * unlocking the mutex is fine.
     */
    mutex_unlock(&isrpipe->mutex);

    return res;
}

int isrpipe_write(isrpipe_t *isrpipe, const uint8_t *buf, size_t n)
{
    int res = tsrb_add(&isrpipe->tsrb, buf, n);

    mutex_unlock(&isrpipe->mutex);

    return res;
}

int isrpipe_read(isrpipe_t *isrpipe, uint8_t *buffer, size_t count)
{
    int res;

    while (!(res = tsrb_get(&isrpipe->tsrb, buffer, count))) {
        mutex_lock(&isrpipe->mutex);
    }
    return res;
}
