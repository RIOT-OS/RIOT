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
#include <stdalign.h>
#include <stdbool.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>

#include "mutex.h"
#include "od.h"
#include "utlist.h"
#include "net/gnrc/pktbuf.h"
#include "net/gnrc/nettype.h"
#include "net/gnrc/pkt.h"
#include "string_utils.h"

#include "pktbuf_internal.h"
#include "pktbuf_static.h"

#define ENABLE_DEBUG 0
#include "debug.h"

static alignas(sizeof(_unused_t)) uint8_t _static_buf[CONFIG_GNRC_PKTBUF_SIZE];
static_assert((CONFIG_GNRC_PKTBUF_SIZE % sizeof(_unused_t)) == 0,
              "CONFIG_GNRC_PKTBUF_SIZE has to be a multiple of 8");
static _unused_t *_first_unused;

#ifdef DEVELHELP
/* maximum number of bytes allocated */
static uint16_t max_byte_count = 0;
#endif

/* internal gnrc_pktbuf functions */
static gnrc_pktsnip_t *_create_snip(gnrc_pktsnip_t *next, const void *data, size_t size,
                                    gnrc_nettype_t type);
static void *_pktbuf_alloc(size_t size);

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
    mutex_lock(&gnrc_pktbuf_mutex);
    if (CONFIG_GNRC_PKTBUF_CHECK_USE_AFTER_FREE) {
        memset(_static_buf, GNRC_PKTBUF_CANARY, sizeof(_static_buf));
    }
    /* Silence false -Wcast-align: _static_buf has qualifier
     * `alignas(_unused_t)`, so it is guaranteed to be safe */
    _first_unused = (_unused_t *)(uintptr_t)_static_buf;
    _first_unused->next = NULL;
    _first_unused->size = sizeof(_static_buf);
    mutex_unlock(&gnrc_pktbuf_mutex);
}

gnrc_pktsnip_t *gnrc_pktbuf_add(gnrc_pktsnip_t *next, const void *data, size_t size,
                                gnrc_nettype_t type)
{
    gnrc_pktsnip_t *pkt;

    if (size > CONFIG_GNRC_PKTBUF_SIZE) {
        DEBUG("pktbuf: size (%" PRIuSIZE ") > CONFIG_GNRC_PKTBUF_SIZE (%u)\n",
              size, CONFIG_GNRC_PKTBUF_SIZE);
        return NULL;
    }
    mutex_lock(&gnrc_pktbuf_mutex);
    pkt = _create_snip(next, data, size, type);
    mutex_unlock(&gnrc_pktbuf_mutex);
    return pkt;
}

gnrc_pktsnip_t *gnrc_pktbuf_mark(gnrc_pktsnip_t *pkt, size_t size, gnrc_nettype_t type)
{
    gnrc_pktsnip_t *marked_snip;
    /* size required for chunk */
    size_t required_new_size = _align(size);
    void *new_data_marked;

    mutex_lock(&gnrc_pktbuf_mutex);
    if ((size == 0) || (pkt == NULL) || (size > pkt->size) || (pkt->data == NULL)) {
        DEBUG("pktbuf: size == 0 (was %" PRIuSIZE ") or pkt == NULL (was %p) or "
              "size > pkt->size (was %" PRIuSIZE ") or pkt->data == NULL (was %p)\n",
              size, (void *)pkt, (pkt ? pkt->size : 0),
              (pkt ? pkt->data : NULL));
        mutex_unlock(&gnrc_pktbuf_mutex);
        return NULL;
    }
    /* create new snip descriptor for marked data */
    marked_snip = _pktbuf_alloc(sizeof(gnrc_pktsnip_t));
    if (marked_snip == NULL) {
        DEBUG("pktbuf: could not reallocate marked section.\n");
        mutex_unlock(&gnrc_pktbuf_mutex);
        return NULL;
    }
    /* marked data would not fit _unused_t marker => move data around to allow
     * for proper free */
    if ((pkt->size != size) && (size < required_new_size)) {
        void *new_data_rest;
        new_data_marked = _pktbuf_alloc(size);
        if (new_data_marked == NULL) {
            DEBUG("pktbuf: could not reallocate marked section.\n");
            gnrc_pktbuf_free_internal(marked_snip, sizeof(gnrc_pktsnip_t));
            mutex_unlock(&gnrc_pktbuf_mutex);
            return NULL;
        }
        new_data_rest = _pktbuf_alloc(pkt->size - size);
        if (new_data_rest == NULL) {
            DEBUG("pktbuf: could not reallocate remaining section.\n");
            gnrc_pktbuf_free_internal(marked_snip, sizeof(gnrc_pktsnip_t));
            gnrc_pktbuf_free_internal(new_data_marked, size);
            mutex_unlock(&gnrc_pktbuf_mutex);
            return NULL;
        }
        memcpy(new_data_marked, pkt->data, size);
        memcpy(new_data_rest, ((uint8_t *)pkt->data) + size, pkt->size - size);
        gnrc_pktbuf_free_internal(pkt->data, pkt->size);
        marked_snip->data = new_data_marked;
        pkt->data = new_data_rest;
    }
    else {
        new_data_marked = pkt->data;
        /* if (pkt->size - size) != 0 take remainder of data, otherwise set NULL */
        pkt->data = (pkt->size != size) ? (((uint8_t *)pkt->data) + size) :
                                          NULL;
    }
    pkt->size -= size;
    _set_pktsnip(marked_snip, pkt->next, new_data_marked, size, type);
    pkt->next = marked_snip;
    mutex_unlock(&gnrc_pktbuf_mutex);
    return marked_snip;
}

int gnrc_pktbuf_realloc_data(gnrc_pktsnip_t *pkt, size_t size)
{
    size_t aligned_size = _align(size);

    mutex_lock(&gnrc_pktbuf_mutex);
    assert(pkt != NULL);
    assert(((pkt->size == 0) && (pkt->data == NULL)) ||
           ((pkt->size > 0) && (pkt->data != NULL) && gnrc_pktbuf_contains(pkt->data)));
    /* new size and old size are equal */
    if (size == pkt->size) {
        /* nothing to do */
        mutex_unlock(&gnrc_pktbuf_mutex);
        return 0;
    }
    /* new size is 0 and data pointer isn't already NULL */
    if ((size == 0) && (pkt->data != NULL)) {
        /* set data pointer to NULL */
        gnrc_pktbuf_free_internal(pkt->data, pkt->size);
        pkt->data = NULL;
    }
    /* if new size is bigger than old size */
    else if (size > pkt->size) {    /* new size does not fit */
        void *new_data = _pktbuf_alloc(size);
        if (new_data == NULL) {
            DEBUG("pktbuf: error allocating new data section\n");
            mutex_unlock(&gnrc_pktbuf_mutex);
            return ENOMEM;
        }
        if (pkt->data != NULL) {            /* if old data exist */
            memcpy(new_data, pkt->data, (pkt->size < size) ? pkt->size : size);
        }
        gnrc_pktbuf_free_internal(pkt->data, pkt->size);
        pkt->data = new_data;
    }
    else if (_align(pkt->size) > aligned_size) {
        gnrc_pktbuf_free_internal(((uint8_t *)pkt->data) + aligned_size,
                     pkt->size - aligned_size);
    }
    pkt->size = size;
    mutex_unlock(&gnrc_pktbuf_mutex);
    return 0;
}

void gnrc_pktbuf_hold(gnrc_pktsnip_t *pkt, unsigned int num)
{
    mutex_lock(&gnrc_pktbuf_mutex);
    while (pkt) {
        assert(pkt->users + num <= 0xff);
        pkt->users += num;
        pkt = pkt->next;
    }
    mutex_unlock(&gnrc_pktbuf_mutex);
}

gnrc_pktsnip_t *gnrc_pktbuf_start_write(gnrc_pktsnip_t *pkt)
{
    mutex_lock(&gnrc_pktbuf_mutex);
    if (pkt == NULL) {
        mutex_unlock(&gnrc_pktbuf_mutex);
        return NULL;
    }

    if (CONFIG_GNRC_PKTBUF_CHECK_USE_AFTER_FREE &&
        pkt->users == GNRC_PKTBUF_CANARY) {
        puts("gnrc_pktbuf: use after free detected\n");
        DEBUG_BREAKPOINT(3);
    }

    if (pkt->users > 1) {
        gnrc_pktsnip_t *new;
        new = _create_snip(pkt->next, pkt->data, pkt->size, pkt->type);
        if (new != NULL) {
            pkt->users--;
        }
        mutex_unlock(&gnrc_pktbuf_mutex);
        return new;
    }
    mutex_unlock(&gnrc_pktbuf_mutex);
    return pkt;
}

#ifdef DEVELHELP
static inline void _print_chunk(void *chunk, size_t size, int num)
{
    printf("=========== chunk %3i (%-10p size: %4" PRIuSIZE ") ===========\n", num, chunk,
           size);
#ifdef MODULE_OD
    od_hex_dump(chunk, size, OD_WIDTH_DEFAULT);
#endif
}

static inline void _print_ptr(_unused_t *ptr)
{
    if (ptr == NULL) {
        printf("(nil)");
    }
    else {
        printf("%p", (void *)ptr);
    }
}

static inline void _print_unused(_unused_t *ptr)
{
    printf("~ unused: ");
    _print_ptr(ptr);
    printf(" (next: ");
    _print_ptr(ptr->next);
    printf(", size: %4u) ~\n", ptr->size);
}

void gnrc_pktbuf_stats(void)
{
    _unused_t *ptr = _first_unused;
    uint8_t *chunk = &_static_buf[0];
    int count = 0;

    printf("packet buffer: first byte: %p, last byte: %p (size: %u)\n",
           (void *)&_static_buf[0],
           (void *)&_static_buf[CONFIG_GNRC_PKTBUF_SIZE],
           CONFIG_GNRC_PKTBUF_SIZE);
    printf("  position of last byte used: %" PRIu16 "\n", max_byte_count);
    if (ptr == NULL) {  /* packet buffer is completely full */
        _print_chunk(chunk, CONFIG_GNRC_PKTBUF_SIZE, count++);
    }

    if (((void *)ptr) == ((void *)chunk)) { /* _first_unused is at the beginning */
        _print_unused(ptr);
        chunk += ptr->size;
        ptr = ptr->next;
    }

    while (ptr) {
        size_t size = ((uint8_t *)ptr) - chunk;
        if ((size == 0) && (!gnrc_pktbuf_contains(ptr)) &&
            (!gnrc_pktbuf_contains(chunk)) && (size > CONFIG_GNRC_PKTBUF_SIZE)) {
            puts("ERROR");
            return;
        }
        _print_chunk(chunk, size, count++);
        chunk += (size + ptr->size);
        _print_unused(ptr);
        ptr = ptr->next;
    }

    if (chunk <= &_static_buf[CONFIG_GNRC_PKTBUF_SIZE - 1]) {
        _print_chunk(chunk, &_static_buf[CONFIG_GNRC_PKTBUF_SIZE] - chunk, count);
    }
}
#endif

#ifdef TEST_SUITES
bool gnrc_pktbuf_is_empty(void)
{
    return ((uintptr_t)_first_unused == (uintptr_t)_static_buf) &&
           (_first_unused->size == sizeof(_static_buf));
}

bool gnrc_pktbuf_is_sane(void)
{
    _unused_t *ptr = _first_unused;

    /* Invariants of this implementation:
     *  - the head of _unused_t list is _first_unused
     *  - if _unused_t list is empty the packet buffer is full and _first_unused is NULL
     *  - forall ptr_in _unused_t list: &_static_buf[0] < ptr
     *                                  && ptr < &_static_buf[CONFIG_GNRC_PKTBUF_SIZE]
     *  - forall ptr in _unused_t list: ptr->next == NULL || ptr < ptr->next
     *  - forall ptr in _unused_t list: (ptr->next != NULL && ptr->size <= (ptr->next - ptr)) ||
     *                                  (ptr->next == NULL
     *                                  && ptr->size == (CONFIG_GNRC_PKTBUF_SIZE - pos_in_buf))
     */

    while (ptr) {
        if ((&_static_buf[0] >= (uint8_t *)ptr)
            && ((uint8_t *)ptr >= &_static_buf[CONFIG_GNRC_PKTBUF_SIZE])) {
            return false;
        }
        if ((ptr->next != NULL) && (ptr >= ptr->next)) {
            return false;
        }
        size_t pos_in_buf = (uint8_t *)ptr - &_static_buf[0];
        if (((ptr->next == NULL) || (ptr->size > (size_t)((uint8_t *)(ptr->next) - (uint8_t *)ptr)))
            && ((ptr->next != NULL) || (ptr->size != CONFIG_GNRC_PKTBUF_SIZE - pos_in_buf))) {
            return false;
        }
        ptr = ptr->next;
    }

    return true;
}
#endif

static gnrc_pktsnip_t *_create_snip(gnrc_pktsnip_t *next, const void *data, size_t size,
                                    gnrc_nettype_t type)
{
    gnrc_pktsnip_t *pkt = _pktbuf_alloc(sizeof(gnrc_pktsnip_t));
    void *_data = NULL;

    if (pkt == NULL) {
        DEBUG("pktbuf: error allocating new packet snip\n");
        return NULL;
    }
    if (size > 0) {
        _data = _pktbuf_alloc(size);
        if (_data == NULL) {
            DEBUG("pktbuf: error allocating data for new packet snip\n");
            gnrc_pktbuf_free_internal(pkt, sizeof(gnrc_pktsnip_t));
            return NULL;
        }
        if (data != NULL) {
            memcpy(_data, data, size);
        }
    }
    _set_pktsnip(pkt, next, _data, size, type);
    return pkt;
}

static void *_pktbuf_alloc(size_t size)
{
    _unused_t *prev = NULL, *ptr = _first_unused;

    size = _align(size);
    while (ptr && (size > ptr->size)) {
        prev = ptr;
        ptr = ptr->next;
    }
    if (ptr == NULL) {
        DEBUG("pktbuf: no space left in packet buffer\n");
        return NULL;
    }
    /* _unused_t struct would fit => add new space at ptr */
    if (sizeof(_unused_t) > (ptr->size - size)) {
        if (prev == NULL) { /* ptr was _first_unused */
            _first_unused = ptr->next;
        }
        else {
            prev->next = ptr->next;
        }
    }
    else {
        /* alignment is ensured by rounding size up in the _align() function.
         * We cast to uintptr_t as intermediate step to silence -Wcast-align */
        _unused_t *new = (_unused_t *)((uintptr_t)ptr + size);

        if (((((uint8_t *)new) - &(_static_buf[0])) + sizeof(_unused_t))
            > CONFIG_GNRC_PKTBUF_SIZE) {
            /* content of new would exceed packet buffer size so set to NULL */
            _first_unused = NULL;
        }
        else if (prev == NULL) { /* ptr was _first_unused */
            _first_unused = new;
        }
        else {
            prev->next = new;
        }
        new->next = ptr->next;
        new->size = ptr->size - size;
    }
#ifdef DEVELHELP
    uint16_t last_byte = (uint16_t)((((uint8_t *)ptr) + size) - &(_static_buf[0]));
    if (last_byte > max_byte_count) {
        max_byte_count = last_byte;
    }
#endif

    const void *mismatch;
    if (CONFIG_GNRC_PKTBUF_CHECK_USE_AFTER_FREE &&
        (mismatch = memchk(ptr + 1, GNRC_PKTBUF_CANARY, size - sizeof(_unused_t)))) {
        printf("[%p] mismatch at offset %"PRIuPTR"/%" PRIuSIZE
               " (ignoring %" PRIuSIZE " initial bytes that were repurposed)\n",
               (void *)ptr, (uintptr_t)mismatch - (uintptr_t)ptr, size, sizeof(_unused_t));
#ifdef MODULE_OD
        od_hex_dump(ptr, size, 0);
#endif
        assert(0);
    }
    if (CONFIG_GNRC_PKTBUF_CHECK_USE_AFTER_FREE) {
        /* clear out canary */
        memset(ptr, ~GNRC_PKTBUF_CANARY, size);
    }

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
    if (CONFIG_GNRC_PKTBUF_CHECK_USE_AFTER_FREE) {
        memset(b, GNRC_PKTBUF_CANARY, sizeof(*b));
    }
    return a;
}

void gnrc_pktbuf_free_internal(void *data, size_t size)
{
    size_t bytes_at_end;
    _unused_t *new = (_unused_t *)data, *prev = NULL, *ptr = _first_unused;

    if (data == NULL) {
        return;
    }

    if (!gnrc_pktbuf_contains(data)) {
        assert(0);
        return;
    }

    if (CONFIG_GNRC_PKTBUF_CHECK_USE_AFTER_FREE) {
        /* check if the data has already been marked as free */
        size_t chk_len = _align(size) - sizeof(*new);
        if (chk_len &&
            !memchk((uint8_t *)data + sizeof(*new), GNRC_PKTBUF_CANARY, chk_len)) {
            printf("pktbuf: double free detected! (at %p, len=%u)\n",
                   data, (unsigned)_align(size));
            DEBUG_BREAKPOINT(2);
        }
        memset(data, GNRC_PKTBUF_CANARY, _align(size));
    }

    while (ptr && (((void *)ptr) < data)) {
        prev = ptr;
        ptr = ptr->next;
    }
    new->next = ptr;
    new->size = _align(size);
    /* calculate number of bytes between new _unused_t chunk and end of packet
     * buffer */
    bytes_at_end = ((&_static_buf[0] + CONFIG_GNRC_PKTBUF_SIZE)
                   - (((uint8_t *)new) + new->size));
    if (bytes_at_end < sizeof(_unused_t)) {
        /* new is very last segment and there is a little bit of memory left
         * that wouldn't fit _unused_t (cut of in _pktbuf_alloc()) => re-add it */
        new->size += bytes_at_end;
    }
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

bool gnrc_pktbuf_contains(void *ptr)
{
    const uintptr_t start = (uintptr_t)_static_buf;
    const uintptr_t end = start + sizeof(_static_buf);
    uintptr_t pos = (uintptr_t)ptr;
    return ((pos >= start) && (pos < end));
}

/** @} */
