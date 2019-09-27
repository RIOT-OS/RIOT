/*
 * Copyright (C) 2019 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file
 * @author  Jose I. Alamos <jose.alamos@haw-hamburg.de>
 */

#include <string.h>
#include "net/netbuf.h"

#if CONFIG_NETBUF_DISABLE_STACK_ALLOC
void *netbuf_stack_alloc(netbuf_t *netbuf, size_t size)
{
    (void) netbuf;
    (void) size;
    return NULL;
}
#endif /* CONFIG_NETBUF_DISABLE_STACK_ALLOC */

void *netbuf_alloc(netbuf_t *netbuf, size_t size)
{
    assert(netbuf);

    if(netbuf->data) {
        return size > netbuf->size ? NULL : netbuf->data;
    }
    else {
        return netbuf_stack_alloc(netbuf, size);
    }
}

void netbuf_init(netbuf_t *netbuf, void *data, size_t max_len)
{
    memset(netbuf, '\0', sizeof(netbuf_t));
    netbuf->data = data;
    netbuf->size = max_len;
}
/** @} */
