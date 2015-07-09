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
        DEBUG("pktbuf: (pkt = %p) not in packet buffer\n", (void *)pkt);
        return ENOENT;
    }

    if (pkt->users > 1 || pkt->next != NULL) {
        DEBUG("pktbuf: more than one user (%u) or pkt->next != NULL (%p)\n",
              pkt->users, (void *)pkt->next);
        return EINVAL;
    }

    mutex_lock(&_pktbuf_mutex);

    new = _pktbuf_internal_realloc(pkt->data, size);

    mutex_unlock(&_pktbuf_mutex);

    if (new == NULL) {
        DEBUG("pktbuf: no buffer space for realloc left\n");
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
        DEBUG("pktbuf: hold (pkt = %p) %u times\n", (void *)pkt, num);
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
            DEBUG("pktbuf: release (pkt = %p)\n", (void *)pkt);
            pkt->users--;
        }

        if (pkt->users == 0) {
            if (_pktbuf_internal_contains(pkt->data)) {
                DEBUG("pktbuf: free pkt->data = %p\n", pkt->data);
                _pktbuf_internal_free(pkt->data);
            }

            if (_pktbuf_internal_contains(pkt)) {
                DEBUG("pktbuf: free pkt = %p\n", (void *)pkt);
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

        DEBUG("pktbuf: pkt->users = %u => copy-on-write\n", pkt->users);
        res = _pktbuf_duplicate(pkt);
        DEBUG("pktbuf: copy-on-write result: (pkt = %p) copied to (res = %p)\n",
              (void *)pkt, (void *)res);

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
    DEBUG("pktbuf: allocated (pkt = %p) ", (void *)pkt);

    if (pkt == NULL) {
        DEBUG("=> failed\n");
        return NULL;
    }

    DEBUG("of size %u\n", (unsigned)sizeof(ng_pktsnip_t));

    pkt->data = _pktbuf_internal_alloc(size);
    DEBUG("pktbuf: allocated (pkt->data = %p) ", pkt->data);

    if (pkt->data == NULL) {
        _pktbuf_internal_free(pkt);
        DEBUG("=> failed (freeing %p)\n", (void *)pkt);

        return NULL;
    }

    DEBUG("of size %u\n", (unsigned)size);
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
    DEBUG("pktbuf: allocated (new_pktsnip = %p) ", (void *)new_pktsnip);

    if (new_pktsnip == NULL) {
        DEBUG("=> failed\n");
        return NULL;
    }

    DEBUG("of size %u\n", (unsigned)sizeof(ng_pktsnip_t));

    if (pkt == NULL || pkt->data != data) {
        if ((size != 0) && (!_pktbuf_internal_contains(data))) {
            new_pktsnip->data = _pktbuf_internal_alloc(size);
            DEBUG("pktbuf: allocated (new_pktsnip->data = %p) ", new_pktsnip->data);

            if (new_pktsnip->data == NULL) {
                _pktbuf_internal_free(new_pktsnip);
                DEBUG("=> failed (freeing %p)\n", (void *)pkt);

                return NULL;
            }

            DEBUG("of size %u\n", (unsigned)size);

            if (data != NULL) {
                DEBUG("pktbuf: copying %u byte from %p to %p\n", (unsigned)size,
                      data, new_pktsnip->data);
                memcpy(new_pktsnip->data, data, size);
            }
        }
        else {
            if (_pktbuf_internal_contains(data)) {
                DEBUG("pktbuf: Adding chunk to %p ", new_pktsnip->data);

                if (!_pktbuf_internal_add_pkt(new_pktsnip->data)) {
                    _pktbuf_internal_free(new_pktsnip);
                    DEBUG("failed (freeing %p)\n", (void *)pkt);

                    return NULL;
                }

                DEBUG("successful\n");
            }

            new_pktsnip->data = data;
            DEBUG("pktbuf: set new_pktsnip->data = %p\n", new_pktsnip->data);
        }

        new_pktsnip->next = NULL;
        LL_PREPEND(pkt, new_pktsnip);
        DEBUG("pktbuf: prepended new_pktsnip to pkt\n");
    }
    else {
        if (size > pkt->size) {
            DEBUG("pktbuf: new size (%u) out of pkt's boundaries (pkt->size = %u)",
                  (unsigned)size, (unsigned)pkt->size);
            return NULL;
        }

        new_pktsnip->next = pkt->next;
        new_pktsnip->data = data;
        DEBUG("pktbuf: set new_pktsnip->data = %p\n", new_pktsnip->data);

        DEBUG("pktbuf: add new_pktsnip (%p) to pkt (%p) after head\n",
              (void *)new_pktsnip, (void *)pkt);
        pkt->next = new_pktsnip;
        pkt->size -= size;
        DEBUG("pktbuf: resize pkt->size to %u\n", (unsigned)pkt->size);
        pkt->data = (void *)(((uint8_t *)pkt->data) + size);
        DEBUG("pktbuf: move pkt->data to %p\n", pkt->data);

        DEBUG("pktbuf: Adding chunk to %p ", pkt->data);

        if (!_pktbuf_internal_add_pkt(pkt->data)) {
            DEBUG("failed (freeing %p)\n", (void *)new_pktsnip);
            _pktbuf_internal_free(new_pktsnip);

            return NULL;
        }

        DEBUG("successful\n");
    }

    new_pktsnip->size = size;
    new_pktsnip->type = type;
    new_pktsnip->users = 1;
    DEBUG("pktbuf: summary of new snip %p: next = %p, data = %p, size = %u, "
          "type = %d, users: %u\n", (void *)new_pktsnip, (void *)new_pktsnip->next,
          new_pktsnip->data, (unsigned)new_pktsnip->size, new_pktsnip->type,
          new_pktsnip->users);

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
