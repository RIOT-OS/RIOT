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

#include <assert.h>
#include <errno.h>
#include <inttypes.h>
#include <stdbool.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>

#include "mutex.h"
#include "od.h"
#include "utlist.h"
#include "net/ng_pktbuf.h"
#include "net/ng_nettype.h"
#include "net/ng_pkt.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#define _ALIGNMENT_MASK    (sizeof(void *) - 1)

typedef struct _unused {
    struct _unused *next;
    unsigned int size;
} _unused_t;

static mutex_t _mutex = MUTEX_INIT;
static uint8_t _pktbuf[NG_PKTBUF_SIZE];
static _unused_t *_first_unused;

/* internal ng_pktbuf functions */
static ng_pktsnip_t *_create_snip(ng_pktsnip_t *next, void *data, size_t size,
                                  ng_nettype_t type);
static void *_pktbuf_alloc(size_t size);
static void _pktbuf_free(void *data, size_t size);

static inline bool _pktbuf_contains(void *ptr)
{
    return (unsigned)((uint8_t *)ptr - _pktbuf) < NG_PKTBUF_SIZE;
}

/* fits size to byte alignment */
static inline size_t _align(size_t size)
{
    return ((size + _ALIGNMENT_MASK) & ~(_ALIGNMENT_MASK));
}


void ng_pktbuf_init(void)
{
    mutex_lock(&_mutex);
    _first_unused = (_unused_t *)_pktbuf;
    _first_unused->next = NULL;
    _first_unused->size = sizeof(_pktbuf);
    mutex_unlock(&_mutex);
}

ng_pktsnip_t *ng_pktbuf_add(ng_pktsnip_t *next, void *data, size_t size,
                            ng_nettype_t type)
{
    ng_pktsnip_t *pkt;
    if ((size == 0) || (size > NG_PKTBUF_SIZE)) {
        DEBUG("pktbuf: size (%u) == 0 || size == NG_PKTBUF_SIZE (%u)\n",
              (unsigned)size, NG_PKTBUF_SIZE);
        return NULL;
    }
    mutex_lock(&_mutex);
    pkt = _create_snip(next, data, size, type);
    mutex_unlock(&_mutex);
    return pkt;
}

ng_pktsnip_t *ng_pktbuf_mark(ng_pktsnip_t *pkt, size_t size, ng_nettype_t type)
{
    ng_pktsnip_t *marked_snip;
    /* size required for chunk */
    size_t required_new_size = (size < sizeof(_unused_t)) ?
                               _align(sizeof(_unused_t)) : _align(size);
    mutex_lock(&_mutex);
    if ((size == 0) || (pkt == NULL) || (size > pkt->size) || (pkt->data == NULL)) {
        DEBUG("pktbuf: size == 0 (was %u) or pkt == NULL (was %p) or "
              "size > pkt->size (was %u) or pkt->data == NULL (was %p)\n",
              (unsigned)size, (void *)pkt, (unsigned)pkt->size, pkt->data);
        mutex_unlock(&_mutex);
        return NULL;
    }
    else if (size == pkt->size) {
        pkt->type = type;
        mutex_unlock(&_mutex);
        return pkt;
    }
    marked_snip = _pktbuf_alloc(sizeof(ng_pktsnip_t));
    if (marked_snip == NULL) {
        DEBUG("pktbuf: could not reallocate marked section.\n");
        mutex_unlock(&_mutex);
        return NULL;
    }
    if (size < required_new_size) { /* would not fit unused marker => move data around */
        void *new_data_marked, *new_data_rest;
        new_data_marked = _pktbuf_alloc(size);
        if (new_data_marked == NULL) {
            DEBUG("pktbuf: could not reallocate marked section.\n");
            _pktbuf_free(marked_snip, sizeof(ng_pktsnip_t));
            mutex_unlock(&_mutex);
            return NULL;
        }
        new_data_rest = _pktbuf_alloc(pkt->size - size);
        if (new_data_rest == NULL) {
            DEBUG("pktbuf: could not reallocate remaining section.\n");
            _pktbuf_free(marked_snip, sizeof(ng_pktsnip_t));
            _pktbuf_free(new_data_marked, size);
            mutex_unlock(&_mutex);
            return NULL;
        }
        memcpy(new_data_marked, pkt->data, size);
        memcpy(new_data_rest, ((uint8_t *)pkt->data) + size, pkt->size - size);
        _pktbuf_free(pkt->data, pkt->size);
        marked_snip->data = new_data_marked;
        pkt->data = new_data_rest;
    }
    else {
        marked_snip->data = pkt->data;
        pkt->data = ((uint8_t *)pkt->data) + size;
    }
    pkt->size -= size;
    marked_snip->next = pkt->next;
    marked_snip->size = size;
    marked_snip->type = type;
    marked_snip->users = 1;
    pkt->next = marked_snip;
    mutex_unlock(&_mutex);
    return marked_snip;
}

int ng_pktbuf_realloc_data(ng_pktsnip_t *pkt, size_t size)
{
    size_t aligned_size = (size < sizeof(_unused_t)) ?
                          _align(sizeof(_unused_t)) : _align(size);
    mutex_lock(&_mutex);
    assert((pkt != NULL) && (pkt->data != NULL) && _pktbuf_contains(pkt->data));
    if (size == 0) {
        DEBUG("pktbuf: size == 0\n");
        mutex_unlock(&_mutex);
        return ENOMEM;
    }
    if (size == pkt->size) {
        mutex_unlock(&_mutex);
        return 0;
    }
    if ((size > pkt->size) ||                               /* new size does not fit */
        ((pkt->size - aligned_size) < sizeof(_unused_t))) { /* resulting hole would not fit marker */
        void *new_data = _pktbuf_alloc(size);
        if (new_data == NULL) {
            DEBUG("pktbuf: error allocating new data section\n");
            mutex_unlock(&_mutex);
            return ENOMEM;
        }
        memcpy(new_data, pkt->data, (pkt->size < size) ? pkt->size : size);
        _pktbuf_free(pkt->data, pkt->size);
        pkt->data = new_data;
    }
    else {
        _pktbuf_free(((uint8_t *)pkt->data) + aligned_size,
                     pkt->size - aligned_size);
    }
    pkt->size = size;
    mutex_unlock(&_mutex);
    return 0;
}

void ng_pktbuf_hold(ng_pktsnip_t *pkt, unsigned int num)
{
    mutex_lock(&_mutex);
    while (pkt) {
        pkt->users += num;
        pkt = pkt->next;
    }
    mutex_unlock(&_mutex);
}

void ng_pktbuf_release(ng_pktsnip_t *pkt)
{
    mutex_lock(&_mutex);
    while (pkt) {
        ng_pktsnip_t *tmp = pkt->next;
        if (pkt->users == 1) {
            pkt->users = 0; /* not necessary but to be on the safe side */
            _pktbuf_free(pkt->data, pkt->size);
            _pktbuf_free(pkt, sizeof(ng_pktsnip_t));
        }
        else {
            pkt->users--;
        }
        pkt = tmp;
    }
    mutex_unlock(&_mutex);
}

ng_pktsnip_t *ng_pktbuf_start_write(ng_pktsnip_t *pkt)
{
    mutex_lock(&_mutex);
    if ((pkt == NULL) || (pkt->size == 0)) {
        mutex_unlock(&_mutex);
        return NULL;
    }
    if (pkt->users > 1) {
        ng_pktsnip_t *new;
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
#ifdef MODULE_OD
static inline void _print_chunk(void *chunk, size_t size, int num)
{
    printf("================ chunk %3d (size: %4u) ================\n", num,
           (unsigned int)size);
    od(chunk, NG_PKTBUF_SIZE, OD_WIDTH_DEFAULT,
       OD_FLAGS_ADDRESS_HEX | OD_FLAGS_BYTES_HEX | OD_FLAGS_LENGTH_1);
}

static inline void _print_unused(_unused_t *ptr)
{
    printf("~ unused: %p (next: %p, size: %4u) ~\n", (void *)ptr,
           (void *)ptr->next, ptr->size);
}
#endif

void ng_pktbuf_stats(void)
{
#ifdef MODULE_OD
    _unused_t *ptr = _first_unused;
    uint8_t *chunk = &_pktbuf[0];
    int count = 0;

    printf("packet buffer: first byte: %p, last byte: %p (size: %u)\n",
           (void *)&_pktbuf[0], (void *)&_pktbuf[NG_PKTBUF_SIZE], NG_PKTBUF_SIZE);
    if (ptr == NULL) {  /* packet buffer is completely full */
        _print_chunk(chunk, NG_PKTBUF_SIZE, count++);
    }

    if (((void *)ptr) == ((void *)chunk)) { /* _first_unused is at the beginning */
        _print_unused(ptr);
        chunk += ptr->size;
        ptr = ptr->next;
    }

    while (ptr) {
        size_t size = ((uint8_t *)ptr) - chunk;
        if ((size == 0) && (!_pktbuf_contains(ptr)) &&
            (!_pktbuf_contains(chunk)) && (size > NG_PKTBUF_SIZE)) {
            puts("ERROR");
            return;
        }
        _print_chunk(chunk, size, count++);
        chunk += (size + ptr->size);
        ptr = ptr->next;
        _print_unused(ptr);
    }

    if (chunk <= &_pktbuf[NG_PKTBUF_SIZE - 1]) {
        _print_chunk(chunk, &_pktbuf[NG_PKTBUF_SIZE] - chunk, count);
    }
#else
    DEBUG("pktbuf: needs od module\n");
#endif
}
#endif

#ifdef TEST_SUITES
bool ng_pktbuf_is_empty(void)
{
    return (_first_unused == (_unused_t *)_pktbuf) &&
           (_first_unused->size == sizeof(_pktbuf));
}

bool ng_pktbuf_is_sane(void)
{
    _unused_t *ptr = _first_unused;

    /* Invariants of this implementation:
     *  - the head of _unused_t list is _first_unused
     *  - if _unused_t list is empty the packet buffer is full and _first_unused is NULL
     *  - forall ptr_in _unused_t list: &_pktbuf[0] < ptr < &_pktbuf[NG_PKTBUF_SIZE]
     *  - forall ptr in _unused_t list: ptr->next == NULL || ptr < ptr->next
     *  - forall ptr in _unused_t list: (ptr->next != NULL && ptr->size <= (ptr->next - ptr)) ||
     *                                  (ptr->next == NULL && ptr->size == (NG_PKTBUF_SIZE - (ptr - &_pktbuf[0])))
     */

    while (ptr) {
        if (&_pktbuf[0] >= (uint8_t *)ptr && (uint8_t *)ptr >= &_pktbuf[NG_PKTBUF_SIZE]) {
            return false;
        }
        if ((ptr->next != NULL) && (ptr >= ptr->next)) {
            return false;
        }
        if (((ptr->next == NULL) || (ptr->size > (size_t)((uint8_t *)(ptr->next) - (uint8_t *)ptr))) &&
            ((ptr->next != NULL) || (ptr->size != (size_t)(NG_PKTBUF_SIZE - ((uint8_t *)ptr - &_pktbuf[0]))))) {
            return false;
        }
        ptr = ptr->next;
    }

    return true;
}
#endif

static ng_pktsnip_t *_create_snip(ng_pktsnip_t *next, void *data, size_t size,
                                  ng_nettype_t type)
{
    ng_pktsnip_t *pkt = _pktbuf_alloc(sizeof(ng_pktsnip_t));
    void *_data;
    if (pkt == NULL) {
        DEBUG("pktbuf: error allocating new packet snip\n");
        return NULL;
    }
    _data = _pktbuf_alloc(size);
    if (_data == NULL) {
        DEBUG("pktbuf: error allocating data for new packet snip\n");
        _pktbuf_free(pkt, sizeof(ng_pktsnip_t));
        return NULL;
    }
    pkt->next = next;
    pkt->size = size;
    pkt->data = _data;
    pkt->type = type;
    pkt->users = 1;
    if (data != NULL) {
        memcpy(_data, data, size);
    }
    return pkt;
}

static void *_pktbuf_alloc(size_t size)
{
    _unused_t *prev = NULL, *ptr = _first_unused;
    size = (size < sizeof(_unused_t)) ? _align(sizeof(_unused_t)) : _align(size);
    while (ptr && size > ptr->size) {
        prev = ptr;
        ptr = ptr->next;
    }
    if (ptr == NULL) {
        DEBUG("pktbuf: no space left in packet buffer\n");
        return NULL;
    }
    if (sizeof(_unused_t) > (ptr->size - size)) {
        if (prev == NULL) { /* ptr was _first_unused */
            _first_unused = ptr->next;
        }
        else {
            prev->next = ptr->next;
        }
    }
    else {
        _unused_t *new = (_unused_t *)(((uint8_t *)ptr) + size);
        if (prev == NULL) { /* ptr was _first_unused */
            _first_unused = new;
        }
        else {
            prev->next = new;
        }
        new->next = ptr->next;
        new->size = ptr->size - size;
    }
    return (void *)ptr;
}

static inline bool _too_small_hole(_unused_t *a, _unused_t *b)
{
    return sizeof(_unused_t) > (size_t)(((uint8_t *)b) - (((uint8_t *)a) + a->size));
}

static inline _unused_t *_merge(_unused_t *a, _unused_t *b)
{
    a->next = b->next;
    a->size = b->size + ((uint8_t *)b - (uint8_t *)a);
    return a;
}

static void _pktbuf_free(void *data, size_t size)
{
    _unused_t *new = (_unused_t *)data, *prev = NULL, *ptr = _first_unused;
    if (!_pktbuf_contains(data)) {
        return;
    }
    while (ptr && (((void *)ptr) < data)) {
        prev = ptr;
        ptr = ptr->next;
    }
    new->next = ptr;
    new->size = (size < sizeof(_unused_t)) ? _align(sizeof(_unused_t)) : _align(size);
    if (prev == NULL) { /* ptr was _first_unused or data before _first_unused */
        _first_unused = new;
    }
    else {
        prev->next = new;
        if (_too_small_hole(prev, new)) {
            new = _merge(prev, new);
        }
    }
    if (_too_small_hole(new, new->next)) {
        _merge(new, new->next);
    }
}

/** @} */
