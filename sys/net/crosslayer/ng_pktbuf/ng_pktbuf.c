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
#include "utlist.h"
#include "net/ng_pktbuf.h"
#include "net/ng_nettype.h"
#include "net/ng_pkt.h"

#include "_pktbuf_internal.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

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

ng_pktsnip_t *ng_pktbuf_add(ng_pktsnip_t *pkt, void *data, size_t size,
                            ng_nettype_t type)
{
    ng_pktsnip_t *new_pktsnip;

    mutex_lock(&_pktbuf_mutex);

    new_pktsnip = _pktbuf_add_unsafe(pkt, data, size, type);

    mutex_unlock(&_pktbuf_mutex);

    return new_pktsnip;
}

void ng_pktbuf_hold(ng_pktsnip_t *pkt, unsigned int num)
{
    if ((pkt == NULL) || (num == 0)) {
        return;
    }

    mutex_lock(&_pktbuf_mutex);

    while (pkt != NULL) {
        pkt->users += num;
        pkt = pkt->next;
    }

    mutex_unlock(&_pktbuf_mutex);
}

void ng_pktbuf_release(ng_pktsnip_t *pkt)
{
    if (pkt == NULL) {
        return;
    }

    mutex_lock(&_pktbuf_mutex);

    while (pkt != NULL) {
        if (pkt->users > 0) {   /* Don't accidentally overshoot */
            pkt->users--;
        }

        if (pkt->users == 0) {
            if (_pktbuf_internal_contains(pkt->data)) {
                _pktbuf_internal_free(pkt->data);
            }

            if (_pktbuf_internal_contains(pkt)) {
                _pktbuf_internal_free(pkt);
            }
        }

        pkt = pkt->next;
    }

    mutex_unlock(&_pktbuf_mutex);
}

ng_pktsnip_t *ng_pktbuf_start_write(ng_pktsnip_t *pkt)
{
    if (pkt != NULL && pkt->users > 1) {
        ng_pktsnip_t *res = NULL;

        mutex_lock(&_pktbuf_mutex);

        res = _pktbuf_duplicate(pkt);

        pkt->users--;

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

static ng_pktsnip_t *_pktbuf_add_unsafe(ng_pktsnip_t *pkt, void *data,
                                        size_t size, ng_nettype_t type)
{
    ng_pktsnip_t *new_pktsnip;

    new_pktsnip = (ng_pktsnip_t *)_pktbuf_internal_alloc(sizeof(ng_pktsnip_t));

    if (new_pktsnip == NULL) {
        return NULL;
    }

    if (pkt == NULL || pkt->data != data) {
        if ((size != 0) && (!_pktbuf_internal_contains(data))) {
            new_pktsnip->data = _pktbuf_internal_alloc(size);

            if (new_pktsnip->data == NULL) {
                _pktbuf_internal_free(new_pktsnip);

                return NULL;
            }

            if (data != NULL) {
                memcpy(new_pktsnip->data, data, size);
            }
        }
        else {
            if (_pktbuf_internal_contains(data)) {
                if (!_pktbuf_internal_add_pkt(new_pktsnip->data)) {
                    _pktbuf_internal_free(new_pktsnip);

                    return NULL;
                }
            }

            new_pktsnip->data = data;
        }

        new_pktsnip->next = NULL;
        LL_PREPEND(pkt, new_pktsnip);
    }
    else {
        if (size > pkt->size) {
            return NULL;
        }

        new_pktsnip->next = pkt->next;
        new_pktsnip->data = data;

        pkt->next = new_pktsnip;
        pkt->size -= size;
        pkt->data = (void *)(((uint8_t *)pkt->data) + size);

        if (!_pktbuf_internal_add_pkt(pkt->data)) {
            _pktbuf_internal_free(new_pktsnip);

            return NULL;
        }
    }

    new_pktsnip->size = size;
    new_pktsnip->type = type;
    new_pktsnip->users = 1;

    return new_pktsnip;
}

static ng_pktsnip_t *_pktbuf_duplicate(const ng_pktsnip_t *pkt)
{
    ng_pktsnip_t *res = NULL;

    res = _pktbuf_alloc(pkt->size);

    if (res == NULL) {
        return NULL;
    }

    DEBUG("pktbuf: copying %u byte from %p to %p\n", (unsigned)pkt->size,
          pkt->data, res->data);
    memcpy(res->data, pkt->data, pkt->size);
    res->type = pkt->type;
    res->next = pkt->next;
    DEBUG("pktbuf: set res->next to %p", (void *)pkt->next);

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
