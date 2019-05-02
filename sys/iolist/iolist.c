/*
 * Copyright (C) 2018 Kaspar Schleiser <kaspar@schleiser.de>
 *               2018 Inria
 *               2018 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @brief       iolist scatter / gather IO
 *
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 * @}
 */

#include <sys/uio.h>

#include "iolist.h"

unsigned iolist_count(const iolist_t *iolist)
{
    unsigned count = 0;
    while (iolist) {
        count++;
        iolist = iolist->iol_next;
    }
    return count;
}

size_t iolist_size(const iolist_t *iolist)
{
    size_t result = 0;
    while (iolist) {
        result += iolist->iol_len;
        iolist = iolist->iol_next;
    }
    return result;
}

size_t iolist_to_iovec(const iolist_t *iolist, struct iovec *iov, unsigned *count)
{
    size_t bytes = 0;
    unsigned _count = 0;

    while (iolist) {
        iov->iov_base = iolist->iol_base;
        iov->iov_len = iolist->iol_len;
        bytes += iov->iov_len;
        _count++;
        iolist = iolist->iol_next;
        iov++;
    }

    *count = _count;

    return bytes;
}
