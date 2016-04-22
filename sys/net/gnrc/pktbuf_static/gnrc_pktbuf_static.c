/*
 * Copyright (C) 2014 Martine Lenders <mlenders@inf.fu-berlin.de>
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
#include <sys/types.h>
#include <sys/uio.h>

#include "mutex.h"
#include "od.h"
#include "utlist.h"
#include "net/gnrc/pktbuf.h"
#include "net/gnrc/nettype.h"
#include "net/gnrc/pkt.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

#define _ALIGNMENT_MASK    (sizeof(void *) - 1)

typedef struct _unused {
    struct _unused *next;
    unsigned int size;
} _unused_t;

static mutex_t _mutex = MUTEX_INIT;
static uint8_t _pktbuf[GNRC_PKTBUF_SIZE];
static _unused_t *_first_unused;

#ifdef DEVELHELP
/* maximum number of bytes allocated */
static uint16_t max_byte_count = 0;
#endif

/* internal gnrc_pktbuf functions */
static gnrc_pktsnip_t *_create_snip(gnrc_pktsnip_t *next, void *data, size_t size,
                                    gnrc_nettype_t type);
static void *_pktbuf_alloc(size_t size);
static void _pktbuf_free(void *data, size_t size);

static inline bool _pktbuf_contains(void *ptr)
{
    return (unsigned)((uint8_t *)ptr - _pktbuf) < GNRC_PKTBUF_SIZE;
}

/* fits size to byte alignment */
static inline size_t _align(size_t size)
{
    return (size + _ALIGNMENT_MASK) & ~(_ALIGNMENT_MASK);
}

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
    mutex_lock(&_mutex);
    _first_unused = (_unused_t *)_pktbuf;
    _first_unused->next = NULL;
    _first_unused->size = sizeof(_pktbuf);
    mutex_unlock(&_mutex);
}

gnrc_pktsnip_t *gnrc_pktbuf_add(gnrc_pktsnip_t *next, void *data, size_t size,
                                gnrc_nettype_t type)
{
    gnrc_pktsnip_t *pkt;

    if ((size == 0) || (size > GNRC_PKTBUF_SIZE)) {
        DEBUG("pktbuf: size (%u) == 0 || size == GNRC_PKTBUF_SIZE (%u)\n",
              (unsigned)size, GNRC_PKTBUF_SIZE);
        return NULL;
    }
    mutex_lock(&_mutex);
    pkt = _create_snip(next, data, size, type);
    mutex_unlock(&_mutex);
    return pkt;
}

gnrc_pktsnip_t *gnrc_pktbuf_mark(gnrc_pktsnip_t *pkt, size_t size, gnrc_nettype_t type)
{
    gnrc_pktsnip_t *marked_snip;
    /* size required for chunk */
    size_t required_new_size = (size < sizeof(_unused_t)) ?
                               _align(sizeof(_unused_t)) : _align(size);
    void *new_data_marked;

    mutex_lock(&_mutex);
    if ((size == 0) || (pkt == NULL) || (size > pkt->size) || (pkt->data == NULL)) {
        DEBUG("pktbuf: size == 0 (was %u) or pkt == NULL (was %p) or "
              "size > pkt->size (was %u) or pkt->data == NULL (was %p)\n",
              (unsigned)size, (void *)pkt, (pkt ? (unsigned)pkt->size : 0),
              (pkt ? pkt->data : NULL));
        mutex_unlock(&_mutex);
        return NULL;
    }
    else if (size == pkt->size) {
        pkt->type = type;
        mutex_unlock(&_mutex);
        return pkt;
    }
    marked_snip = _pktbuf_alloc(sizeof(gnrc_pktsnip_t));
    if (marked_snip == NULL) {
        DEBUG("pktbuf: could not reallocate marked section.\n");
        mutex_unlock(&_mutex);
        return NULL;
    }
    /* would not fit unused marker => move data around */
    if ((size < required_new_size) || ((pkt->size - size) < sizeof(_unused_t))) {
        void *new_data_rest;
        new_data_marked = _pktbuf_alloc(size);
        if (new_data_marked == NULL) {
            DEBUG("pktbuf: could not reallocate marked section.\n");
            _pktbuf_free(marked_snip, sizeof(gnrc_pktsnip_t));
            mutex_unlock(&_mutex);
            return NULL;
        }
        new_data_rest = _pktbuf_alloc(pkt->size - size);
        if (new_data_rest == NULL) {
            DEBUG("pktbuf: could not reallocate remaining section.\n");
            _pktbuf_free(marked_snip, sizeof(gnrc_pktsnip_t));
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
        new_data_marked = pkt->data;
        pkt->data = ((uint8_t *)pkt->data) + size;
    }
    pkt->size -= size;
    _set_pktsnip(marked_snip, pkt->next, new_data_marked, size, type);
    pkt->next = marked_snip;
    mutex_unlock(&_mutex);
    return marked_snip;
}

int gnrc_pktbuf_realloc_data(gnrc_pktsnip_t *pkt, size_t size)
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
        if (_align(pkt->size) > aligned_size) {
            _pktbuf_free(((uint8_t *)pkt->data) + aligned_size,
                         pkt->size - aligned_size);
        }
    }
    pkt->size = size;
    mutex_unlock(&_mutex);
    return 0;
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
        assert(_pktbuf_contains(pkt));
        tmp = pkt->next;
        if (pkt->users == 1) {
            pkt->users = 0; /* not necessary but to be on the safe side */
            _pktbuf_free(pkt->data, pkt->size);
            _pktbuf_free(pkt, sizeof(gnrc_pktsnip_t));
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
    if ((pkt == NULL) || (pkt->size == 0)) {
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

gnrc_pktsnip_t *gnrc_pktbuf_get_iovec(gnrc_pktsnip_t *pkt, size_t *len)
{
    size_t length;
    gnrc_pktsnip_t *head;
    struct iovec *vec;

    if (pkt == NULL) {
        *len = 0;
        return NULL;
    }

    /* count the number of snips in the packet and allocate the IOVEC */
    length = gnrc_pkt_count(pkt);
    head = gnrc_pktbuf_add(pkt, NULL, (length * sizeof(struct iovec)),
                           GNRC_NETTYPE_IOVEC);
    if (head == NULL) {
        *len = 0;
        return NULL;
    }
    vec = (struct iovec *)(head->data);
    /* fill the IOVEC */
    while (pkt != NULL) {
        vec->iov_base = pkt->data;
        vec->iov_len = pkt->size;
        ++vec;
        pkt = pkt->next;
    }
    *len = length;
    return head;
}

#ifdef DEVELHELP
#ifdef MODULE_OD
static inline void _print_chunk(void *chunk, size_t size, int num)
{
    printf("=========== chunk %3d (%-10p size: %4u) ===========\n", num, chunk,
           (unsigned int)size);
    od(chunk, size, OD_WIDTH_DEFAULT,
       OD_FLAGS_ADDRESS_HEX | OD_FLAGS_BYTES_HEX | OD_FLAGS_LENGTH_1);
}

static inline void _print_unused(_unused_t *ptr)
{
    printf("~ unused: %p (next: %p, size: %4u) ~\n", (void *)ptr,
           (void *)ptr->next, ptr->size);
}
#endif

void gnrc_pktbuf_stats(void)
{
#ifdef MODULE_OD
    _unused_t *ptr = _first_unused;
    uint8_t *chunk = &_pktbuf[0];
    int count = 0;

    printf("packet buffer: first byte: %p, last byte: %p (size: %u)\n",
           (void *)&_pktbuf[0], (void *)&_pktbuf[GNRC_PKTBUF_SIZE], GNRC_PKTBUF_SIZE);
    printf("  position of last byte used: %" PRIu16 "\n", max_byte_count);
    if (ptr == NULL) {  /* packet buffer is completely full */
        _print_chunk(chunk, GNRC_PKTBUF_SIZE, count++);
    }

    if (((void *)ptr) == ((void *)chunk)) { /* _first_unused is at the beginning */
        _print_unused(ptr);
        chunk += ptr->size;
        ptr = ptr->next;
    }

    while (ptr) {
        size_t size = ((uint8_t *)ptr) - chunk;
        if ((size == 0) && (!_pktbuf_contains(ptr)) &&
            (!_pktbuf_contains(chunk)) && (size > GNRC_PKTBUF_SIZE)) {
            puts("ERROR");
            return;
        }
        _print_chunk(chunk, size, count++);
        chunk += (size + ptr->size);
        _print_unused(ptr);
        ptr = ptr->next;
    }

    if (chunk <= &_pktbuf[GNRC_PKTBUF_SIZE - 1]) {
        _print_chunk(chunk, &_pktbuf[GNRC_PKTBUF_SIZE] - chunk, count);
    }
#else
    DEBUG("pktbuf: needs od module\n");
#endif
}
#endif

#ifdef TEST_SUITES
bool gnrc_pktbuf_is_empty(void)
{
    return (_first_unused == (_unused_t *)_pktbuf) &&
           (_first_unused->size == sizeof(_pktbuf));
}

bool gnrc_pktbuf_is_sane(void)
{
    _unused_t *ptr = _first_unused;

    /* Invariants of this implementation:
     *  - the head of _unused_t list is _first_unused
     *  - if _unused_t list is empty the packet buffer is full and _first_unused is NULL
     *  - forall ptr_in _unused_t list: &_pktbuf[0] < ptr < &_pktbuf[GNRC_PKTBUF_SIZE]
     *  - forall ptr in _unused_t list: ptr->next == NULL || ptr < ptr->next
     *  - forall ptr in _unused_t list: (ptr->next != NULL && ptr->size <= (ptr->next - ptr)) ||
     *                                  (ptr->next == NULL && ptr->size == (GNRC_PKTBUF_SIZE - (ptr - &_pktbuf[0])))
     */

    while (ptr) {
        if (&_pktbuf[0] >= (uint8_t *)ptr && (uint8_t *)ptr >= &_pktbuf[GNRC_PKTBUF_SIZE]) {
            return false;
        }
        if ((ptr->next != NULL) && (ptr >= ptr->next)) {
            return false;
        }
        if (((ptr->next == NULL) || (ptr->size > (size_t)((uint8_t *)(ptr->next) - (uint8_t *)ptr))) &&
            ((ptr->next != NULL) ||
             (ptr->size != (size_t)(GNRC_PKTBUF_SIZE - ((uint8_t *)ptr - &_pktbuf[0]))))) {
            return false;
        }
        ptr = ptr->next;
    }

    return true;
}
#endif

static gnrc_pktsnip_t *_create_snip(gnrc_pktsnip_t *next, void *data, size_t size,
                                    gnrc_nettype_t type)
{
    gnrc_pktsnip_t *pkt = _pktbuf_alloc(sizeof(gnrc_pktsnip_t));
    void *_data;

    if (pkt == NULL) {
        DEBUG("pktbuf: error allocating new packet snip\n");
        return NULL;
    }
    _data = _pktbuf_alloc(size);
    if (_data == NULL) {
        DEBUG("pktbuf: error allocating data for new packet snip\n");
        _pktbuf_free(pkt, sizeof(gnrc_pktsnip_t));
        return NULL;
    }
    _set_pktsnip(pkt, next, _data, size, type);
    if (data != NULL) {
        memcpy(_data, data, size);
    }
    return pkt;
}

static void *_pktbuf_alloc(size_t size)
{
    _unused_t *prev = NULL, *ptr = _first_unused;

    size = (size < sizeof(_unused_t)) ? _align(sizeof(_unused_t)) : _align(size);
    while (ptr && (size > ptr->size)) {
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
#ifdef DEVELHELP
    uint16_t last_byte = (uint16_t)((((uint8_t *)ptr) + size) - &(_pktbuf[0]));
    if (last_byte > max_byte_count) {
        max_byte_count = last_byte;
    }
#endif
    return (void *)ptr;
}

static inline bool _too_small_hole(_unused_t *a, _unused_t *b)
{
    return sizeof(_unused_t) > (size_t)(((uint8_t *)b) - (((uint8_t *)a) + a->size));
}

static inline _unused_t *_merge(_unused_t *a, _unused_t *b)
{
    assert(b != NULL);

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
    if ((new->next != NULL) && (_too_small_hole(new, new->next))) {
        _merge(new, new->next);
    }
}


gnrc_pktsnip_t *gnrc_pktbuf_remove_snip(gnrc_pktsnip_t *pkt, gnrc_pktsnip_t *snip)
{
    LL_DELETE(pkt, snip);
    snip->next = NULL;
    gnrc_pktbuf_release(snip);

    return pkt;
}

gnrc_pktsnip_t *gnrc_pktbuf_replace_snip(gnrc_pktsnip_t *pkt, gnrc_pktsnip_t *old, gnrc_pktsnip_t *add)
{
    /* If add is a list we need to preserve its tail */
    if (add->next != NULL) {
        gnrc_pktsnip_t *tail = add->next;
        gnrc_pktsnip_t *back;
        LL_SEARCH_SCALAR(tail, back, next, NULL); /* find the last snip in add */
        /* Replace old */
        LL_REPLACE_ELEM(pkt, old, add);
        /* and wire in the tail between */
        back->next = add->next;
        add->next = tail;
    }
    else {
        /* add is a single element, has no tail, simply replace */
        LL_REPLACE_ELEM(pkt, old, add);
    }
    old->next = NULL;
    gnrc_pktbuf_release(old);

    return pkt;
}

gnrc_pktsnip_t *gnrc_pktbuf_duplicate_upto(gnrc_pktsnip_t *pkt, gnrc_nettype_t type)
{
    mutex_lock(&_mutex);

    bool is_shared = pkt->users > 1;
    size_t size = gnrc_pkt_len_upto(pkt, type);

    DEBUG("ipv6_ext: duplicating %d octets\n", (int) size);

    gnrc_pktsnip_t *tmp;
    gnrc_pktsnip_t *target = gnrc_pktsnip_search_type(pkt, type);
    gnrc_pktsnip_t *next = (target == NULL) ? NULL : target->next;
    gnrc_pktsnip_t *new = _create_snip(next, NULL, size, type);

    if (new == NULL) {
        mutex_unlock(&_mutex);

        return NULL;
    }

    /* copy payloads */
    for (tmp = pkt; tmp != NULL; tmp = tmp->next) {
        uint8_t *dest = ((uint8_t *)new->data) + (size - tmp->size);

        memcpy(dest, tmp->data, tmp->size);

        size -= tmp->size;

        if (tmp->type == type) {
            break;
        }
    }

    /* decrements reference counters */

    if (target != NULL) {
        target->next = NULL;
    }

    _release_error_locked(pkt, GNRC_NETERR_SUCCESS);

    if (is_shared && (target != NULL)) {
        target->next = next;
    }

    mutex_unlock(&_mutex);

    return new;
}

/** @} */
