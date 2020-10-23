/*
 * Copyright (C) 2017 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup net_gnrc_pktbuf
 * @{
 *
 * @file
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#include <assert.h>
#include <errno.h>
#include <inttypes.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/uio.h>

#include "log.h"
#include "mutex.h"
#include "od.h"
#include "utlist.h"
#include "net/gnrc/pktbuf.h"
#include "net/gnrc/nettype.h"
#include "net/gnrc/pkt.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static mutex_t _mutex = MUTEX_INIT;

#ifdef MODULE_FUZZING
extern gnrc_pktsnip_t *gnrc_pktbuf_fuzzptr;
#endif

#if defined(TEST_SUITES) || defined(MODULE_FUZZING)
static unsigned mallocs;

static inline void *_malloc(size_t size)
{
    mallocs++;
    return malloc(size);
}

static inline void _free(void *ptr)
{
    if (ptr != NULL) {
        /* The fuzzing module is only enabled when building a fuzzing
         * application from the fuzzing/ subdirectory. If _free is
         * called on the crafted fuzzing packet, the setup assumes that
         * input processing has completed and the application terminates. */
#if defined(MODULE_FUZZING) && !defined(MODULE_GNRC_SOCK)
        if (ptr == gnrc_pktbuf_fuzzptr) {
           exit(EXIT_SUCCESS);
        }
#endif
        mallocs--;
        free(ptr);
    }
}
#else
#define _malloc(size)   malloc(size)
#define _free(ptr)      free(ptr)
#endif

/* internal gnrc_pktbuf functions */
static gnrc_pktsnip_t *_create_snip(gnrc_pktsnip_t *next, const void *data, size_t size,
                                    gnrc_nettype_t type);

static inline void _set_pktsnip(gnrc_pktsnip_t *pkt, gnrc_pktsnip_t *next,
                                void *data, size_t size, gnrc_nettype_t type)
{
    pkt->next = next;
    pkt->data = data;
    pkt->size = size;
    pkt->type = type;
    pkt->users = 1;
#ifdef MODULE_GNRC_NETERR
    pkt->err_sub = KERNEL_PID_UNDEF;
#endif
}

void gnrc_pktbuf_init(void)
{
#ifdef TEST_SUITES
    mallocs = 0;
#endif
}

gnrc_pktsnip_t *gnrc_pktbuf_add(gnrc_pktsnip_t *next, const void *data, size_t size,
                                gnrc_nettype_t type)
{
    gnrc_pktsnip_t *pkt;

    if (size > CONFIG_GNRC_PKTBUF_SIZE) {
        DEBUG("pktbuf: size (%u) > CONFIG_GNRC_PKTBUF_SIZE (%u)\n",
              (unsigned)size, CONFIG_GNRC_PKTBUF_SIZE);
        return NULL;
    }
    mutex_lock(&_mutex);
    pkt = _create_snip(next, data, size, type);
    mutex_unlock(&_mutex);
    return pkt;
}

static gnrc_pktsnip_t *_mark(gnrc_pktsnip_t *pkt, size_t size, gnrc_nettype_t type)
{
    gnrc_pktsnip_t *header;
    void *header_data, *payload;

    if ((size == 0) || (pkt == NULL) || (size > pkt->size) || (pkt->data == NULL)) {
        DEBUG("pktbuf: size == 0 (was %u) or pkt == NULL (was %p) or "
              "size > pkt->size (was %u) or pkt->data == NULL (was %p)\n",
              (unsigned)size, (void *)pkt, (pkt ? (unsigned)pkt->size : 0),
              (pkt ? pkt->data : NULL));
        return NULL;
    }
    /* create new snip descriptor for marked data */
    header = _malloc(sizeof(gnrc_pktsnip_t));
    if (header == NULL) {
        DEBUG("pktbuf: could not reallocate marked section.\n");
        return NULL;
    }
    if (pkt->size == size) {
        _set_pktsnip(header, pkt->next, pkt->data, size, type);
        _set_pktsnip(pkt, header, NULL, 0, pkt->type);
        return header;
    }
    /* we can not just "snip off" something from the end of a malloc'd section
     * so we need to realloc for marked snip */
    payload = _malloc(pkt->size - size);
    if (payload == NULL) {
        DEBUG("pktbuf: could not reallocate marked section.\n");
        _free(header);
        return NULL;
    }
    memcpy(payload, ((uint8_t *)pkt->data) + size, pkt->size - size);
    header_data = realloc(pkt->data, size);
    if (header_data == NULL) {
        DEBUG("pktbuf: could not reallocate marked section.\n");
        _free(payload);
        _free(header);
        return NULL;
    }
    pkt->data = payload;
    pkt->size -= size;
    _set_pktsnip(header, pkt->next, header_data, size, type);
    pkt->next = header;
    return header;
}

gnrc_pktsnip_t *gnrc_pktbuf_mark(gnrc_pktsnip_t *pkt, size_t size, gnrc_nettype_t type)
{
    gnrc_pktsnip_t *new;

    mutex_lock(&_mutex);
    new = _mark(pkt, size, type);
    mutex_unlock(&_mutex);
    return new;
}

static int _realloc_data(gnrc_pktsnip_t *pkt, size_t size)
{
    assert(pkt != NULL);
    assert(((pkt->size == 0) && (pkt->data == NULL)) ||
           ((pkt->size > 0) && (pkt->data != NULL)));
    /* new size and old size are equal */
    if (size == pkt->size) {
        /* nothing to do */
        return 0;
    }
    /* new size is 0 and data pointer isn't already NULL */
    if ((size == 0) && (pkt->data != NULL)) {
        /* set data pointer to NULL */
        _free(pkt->data);
        pkt->data = NULL;
    }
    else {
        void *data = (pkt->data) ? realloc(pkt->data, size) : _malloc(size);
        if (data == NULL) {
            DEBUG("pktbuf: error allocating new data section\n");
            return ENOMEM;
        }
        pkt->data = data;
    }
    pkt->size = size;
    return 0;
}

int gnrc_pktbuf_realloc_data(gnrc_pktsnip_t *pkt, size_t size)
{
    int res;

    mutex_lock(&_mutex);
    res = _realloc_data(pkt, size);
    mutex_unlock(&_mutex);
    return res;
}

void gnrc_pktbuf_hold(gnrc_pktsnip_t *pkt, unsigned int num)
{
    mutex_lock(&_mutex);
    while (pkt) {
        pkt->users += num;
        pkt = pkt->next;
    }
    mutex_unlock(&_mutex);
}

static void _release_error_locked(gnrc_pktsnip_t *pkt, uint32_t err)
{
    while (pkt) {
        gnrc_pktsnip_t *tmp;
        tmp = pkt->next;
        if (pkt->users == 1) {
            pkt->users = 0; /* not necessary but to be on the safe side */
            _free(pkt->data);
            _free(pkt);
        }
        else {
            pkt->users--;
        }
        DEBUG("pktbuf: report status code %" PRIu32 "\n", err);
        gnrc_neterr_report(pkt, err);
        pkt = tmp;
    }
}

void gnrc_pktbuf_release_error(gnrc_pktsnip_t *pkt, uint32_t err)
{
    mutex_lock(&_mutex);
    _release_error_locked(pkt, err);
    mutex_unlock(&_mutex);
}

gnrc_pktsnip_t *gnrc_pktbuf_start_write(gnrc_pktsnip_t *pkt)
{
    mutex_lock(&_mutex);
    if (pkt == NULL) {
        mutex_unlock(&_mutex);
        return NULL;
    }
    if (pkt->users > 1) {
        gnrc_pktsnip_t *new;
        new = _create_snip(pkt->next, pkt->data, pkt->size, pkt->type);
        if (new != NULL) {
            pkt->users--;
        }
        mutex_unlock(&_mutex);
        return new;
    }
    mutex_unlock(&_mutex);
    return pkt;
}

#ifdef DEVELHELP
void gnrc_pktbuf_stats(void)
{
    LOG_INFO("pktbuf: no stat output for gnrc_pktbuf_malloc, use tools like valgrind\n");
}
#endif

#ifdef TEST_SUITES
bool gnrc_pktbuf_is_empty(void)
{
    /* assert always true, use valgrind to check */
    return (mallocs == 0);
}

bool gnrc_pktbuf_is_sane(void)
{
    /* assert always true, use valgrind to check */
    return true;
}
#endif

static gnrc_pktsnip_t *_create_snip(gnrc_pktsnip_t *next, const void *data, size_t size,
                                    gnrc_nettype_t type)
{
    gnrc_pktsnip_t *pkt = _malloc(sizeof(gnrc_pktsnip_t));
    void *_data = NULL;

    if (pkt == NULL) {
        DEBUG("pktbuf: error allocating new packet snip\n");
        return NULL;
    }
    if (size > 0) {
        _data = _malloc(size);
        if (_data == NULL) {
            DEBUG("pktbuf: error allocating data for new packet snip\n");
            _free(pkt);
            return NULL;
        }
    }
    _set_pktsnip(pkt, next, _data, size, type);
    if (data != NULL) {
        memcpy(_data, data, size);
    }
    return pkt;
}

/** @} */
