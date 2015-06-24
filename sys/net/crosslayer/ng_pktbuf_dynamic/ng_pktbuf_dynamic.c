/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_ng_pktbuf
 * @{
 *
 * @file
 * @brief       Pktbuf implementation based on dynamic memory allocation
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 * @}
 */

#include <stdlib.h>
#include <string.h>
#include <errno.h>

#include "mutex.h"
#include "net/ng_pkt.h"

#define ENABLE_DEBUG    (1)
#include "debug.h"

#ifdef TEST_SUITES
static size_t count = 0;
static size_t used_sum = 0;
#endif

#ifdef TEST_SUITES
#define STATS_INC(x)        stats_inc(x)
#define STATS_DEC(x)        stats_dec(x)
#else
#define STATS_INC(x)
#define STATS_DEC(x)
#endif

static mutex_t lock = MUTEX_INIT;

#ifdef TEST_SUITES
static inline void stats_inc(size_t size)
{
    ++count;
    used_sum += size;
}

static inline void stats_dec(size_t size)
{
    --count;
    used_sum -= size;
}
#endif

static void _init(ng_pktsnip_t *snip, void *data, size_t size,
                  ng_nettype_t type, ng_pktsnip_t *pkt)
{
    snip->users = 1;
    snip->next = pkt;
    snip->data = data;
    snip->size = size;
    snip->type = type;
}

static ng_pktsnip_t *_alloc(void *data, size_t size,
                         ng_nettype_t type, ng_pktsnip_t *pkt)
{
    ng_pktsnip_t *snip;
    void *tmp;

    snip = (ng_pktsnip_t *)malloc(sizeof(ng_pktsnip_t));
    tmp = malloc(size);
    if ((snip == NULL) || (tmp == NULL)) {
        free(snip);
        free(tmp);
        return NULL;
    }

    STATS_INC(size);
    _init(snip, tmp, size, type, pkt);
    if (data) {
        memcpy(snip->data, data, size);
    }
    return snip;
}

ng_pktsnip_t *ng_pktbuf_add(ng_pktsnip_t *pkt, void *data, size_t size,
                            ng_nettype_t type)
{
    if (pkt && data && pkt->data == data && pkt->size > size) {
        ng_pktsnip_t *snip;
        void *a, *b;
        snip = malloc(sizeof(ng_pktsnip_t));
        a = malloc(size);
        b = malloc(pkt->size - size);
        if ((snip == NULL) || (a == NULL) || (b == NULL)) {
            free(snip);
            free(a);
            free(b);
            return NULL;
        }

        memcpy(a, data, size);
        memcpy(b, ((char *)data) + size, pkt->size - size);
        free(pkt->data);

        _init(snip, a, size, type, pkt->next);
        _init(pkt, b, pkt->size - size, pkt->type, snip);
#ifdef TEST_SUITES
        ++count;
#endif
        return snip;
    }
    return _alloc(data, size, type, pkt);
}

int ng_pktbuf_realloc_data(ng_pktsnip_t *pkt, size_t size)
{
    void *tmp;

    if (pkt == NULL || size == 0 || pkt->users > 1 || pkt->next != NULL) {
        return -EINVAL;
    }

    tmp = malloc(size);
    if (tmp == NULL) {
        return -ENOMEM;
    }

    memcpy(tmp, pkt->data, (size >= pkt->size) ? pkt->size : size);
    free(pkt->data);
#ifdef TEST_SUITES
    used_sum += size;
    used_sum -= pkt->size;
#endif
    pkt->data = tmp;
    pkt->size = size;
    return 0;
}

void ng_pktbuf_hold(ng_pktsnip_t *pkt, unsigned int num)
{
    mutex_lock(&lock);
    while (pkt) {
        pkt->users += num;
        pkt = pkt->next;
    }
    mutex_unlock(&lock);
}

void ng_pktbuf_release(ng_pktsnip_t *pkt)
{
    mutex_lock(&lock);
    while (pkt) {
        ng_pktsnip_t *tmp = pkt->next;
        if (pkt->users == 1) {
            STATS_DEC(pkt->size);
            free(pkt->data);
            free((void *)pkt);
        }
        else {
            --pkt->users;
        }
        pkt = tmp;
    }
    mutex_unlock(&lock);
}

ng_pktsnip_t *ng_pktbuf_start_write(ng_pktsnip_t *pkt)
{
    if (pkt) {
        mutex_lock(&lock);
        if (pkt->users == 1) {
            mutex_unlock(&lock);
            return pkt;
        }
        ng_pktsnip_t *res = _alloc(pkt->data, pkt->size, pkt->type, pkt->next);
        if (res) {
            --pkt->users;
        }
        mutex_unlock(&lock);
        return res;
    }
    return NULL;
}

#ifdef DEVELHELP
void ng_pktbuf_stats(void)
{
    /* TODO */
}
#endif


/* for testing */
#ifdef TEST_SUITES
bool ng_pktbuf_is_empty(void)
{
    return used_sum == 0;
}

void ng_pktbuf_reset(void) {
    count = 0;
    used_sum = 0;
}
#endif
