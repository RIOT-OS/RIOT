/*
 * Copyright (C) 2014 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup net_ng_pktbuf
 * @{
 *
 * @file
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#include <errno.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "clist.h"
#include "mutex.h"
#include "net/ng_nettype.h"
#include "net/ng_pkt.h"
#include "utlist.h"

#include "net/ng_pktbuf.h"

#include "_pktbuf_internal.h"

static mutex_t _pktbuf_mutex = MUTEX_INIT;

/* internal ng_pktbuf functions */
static ng_pktsnip_t *_pktbuf_alloc(size_t size);
static ng_pktsnip_t *_pktbuf_add_unsafe(ng_pktsnip_t *pkt, void *data,
                                        size_t size, ng_nettype_t type);
static ng_pktsnip_t *_pktbuf_duplicate(const ng_pktsnip_t *pkt);

int ng_pktbuf_realloc_data(ng_pktsnip_t *pkt, size_t size)
{
    void *new;

    if (pkt == NULL || !_pktbuf_internal_contains(pkt->data)) {
        return ENOENT;
    }

    if (pkt->users > 1 || pkt->next != NULL) {
        return EINVAL;
    }

    mutex_lock(&_pktbuf_mutex);

    new = _pktbuf_internal_realloc(pkt->data, size);

    mutex_unlock(&_pktbuf_mutex);

    if (new == NULL) {
        return ENOMEM;
    }

    pkt->data = new;
    pkt->size = size;

    return 0;
}

ng_pktsnip_t *ng_pktbuf_add(ng_pktsnip_t *next, void *data, size_t size,
                            ng_nettype_t type)
{
    ng_pktsnip_t *snip;

    mutex_lock(&_pktbuf_mutex);

    snip = _pktbuf_add_unsafe(next, data, size, type);

    mutex_unlock(&_pktbuf_mutex);

    return snip;
}

void ng_pktbuf_release(ng_pktsnip_t *pkt)
{
    if (pkt == NULL) {
        return;
    }

    atomic_set_return(&(pkt->users), pkt->users - 1);

    if (pkt->users == 0 && _pktbuf_internal_contains(pkt->data)) {
        mutex_lock(&_pktbuf_mutex);

        _pktbuf_internal_free(pkt->data);
        _pktbuf_internal_free(pkt);

        mutex_unlock(&_pktbuf_mutex);
    }
}

ng_pktsnip_t *ng_pktbuf_start_write(ng_pktsnip_t *pkt)
{
    if (pkt != NULL && pkt->users > 1) {
        ng_pktsnip_t *res = NULL;

        mutex_lock(&_pktbuf_mutex);

        res = _pktbuf_duplicate(pkt);

        atomic_set_return(&pkt->users, pkt->users - 1);

        mutex_unlock(&_pktbuf_mutex);

        return res;
    }

    return pkt;
}

/***********************************
 * internal ng_pktbuf functions       *
 ***********************************/

static ng_pktsnip_t *_pktbuf_alloc(size_t size)
{
    ng_pktsnip_t *pkt;

    pkt = (ng_pktsnip_t *)_pktbuf_internal_alloc(sizeof(ng_pktsnip_t));

    if (pkt == NULL) {
        return NULL;
    }

    pkt->data = _pktbuf_internal_alloc(size);

    if (pkt->data == NULL) {
        _pktbuf_internal_free(pkt);

        return NULL;
    }

    pkt->next = NULL;
    pkt->size = size;
    pkt->users = 1;

    return pkt;
}

static ng_pktsnip_t *_pktbuf_add_unsafe(ng_pktsnip_t *next, void *data,
                                        size_t size, ng_nettype_t type)
{
    ng_pktsnip_t *snip;

    if (size == 0) {
        data = 0;
    }

    snip = (ng_pktsnip_t *)_pktbuf_internal_alloc(sizeof(ng_pktsnip_t));

    if (snip == NULL) {
        return NULL;
    }

    if (next == NULL || next->data != data) {
        if (size != 0) {
            snip->data = _pktbuf_internal_alloc(size);

            if (snip->data == NULL) {
                _pktbuf_internal_free(snip);

                return NULL;
            }

            if (data != NULL) {
                memcpy(snip->data, data, size);
            }
        }
        else {
            snip->data = data;
        }
    }
    else {
        snip->data = data;

        next->size -= size;
        next->data = (void *)(((uint8_t *)next->data) + size);

        if (!_pktbuf_internal_add_pkt(next->data)) {
            _pktbuf_internal_free(snip);

            return NULL;
        }
    }

    snip->next = NULL;
    snip->size = size;
    snip->type = type;
    snip->users = 1;

    LL_PREPEND(next, snip);

    return snip;
}

static ng_pktsnip_t *_pktbuf_duplicate(const ng_pktsnip_t *pkt)
{
    ng_pktsnip_t *res = NULL;

    res = _pktbuf_alloc(pkt->size);

    if (res == NULL) {
        return NULL;
    }

    memcpy(res->data, pkt->data, pkt->size);
    res->type = pkt->type;

    while (pkt->next) {
        ng_pktsnip_t *header = NULL;

        pkt = pkt->next;
        header = _pktbuf_add_unsafe(res, pkt->data, pkt->size, pkt->type);

        if (header == NULL) {
            do {
                ng_pktsnip_t *next = res->next;

                _pktbuf_internal_free(res->data);
                _pktbuf_internal_free(res);

                res = next;
            } while (res);

            return NULL;
        }
    }

    return res;
}

#ifdef DEVELHELP
void ng_pktbuf_stats(void)
{
    _pktbuf_internal_stats();
}
#endif

#ifdef TEST_SUITES
bool ng_pktbuf_is_empty(void)
{
    return _pktbuf_internal_is_empty();
}

void ng_pktbuf_reset(void)
{
    _pktbuf_internal_reset();
}
#endif

/** @} */
