/*
 * Copyright (C) 2015 Martin Lenders <mlenders@inf.fu-berlin.de>
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

#include <inttypes.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "od.h"
#include "net/ng_pktbuf.h"
#include "_pktbuf_internal.h"

/* only for static packet buffer */
#if NG_PKTBUF_SIZE > 0

#define _PKTBUF_ALIGN_BYTES (sizeof(void *))

#ifdef DEVELHELP
static unsigned int _pktbuf_max_bytes = 0;
#endif

/**
 * @brief   Data type to represent used chunks in packet buffer.
 */
typedef struct __attribute__((packed)) _used_t {
    struct _used_t *next;
    uint16_t size;
    uint8_t pkts;
    uint8_t _align;     /* alignment */
} _used_t;

static uint8_t _buf[NG_PKTBUF_SIZE];

/**
 * @brief   Get first element in buffer
 */
static inline _used_t *_head(void)
{
    return (_used_t *)_buf;
}

/**
 * @brief Get data part (memory behind `_used_t` descriptive header) of a packet
 */
static inline void *_data(_used_t *node)
{
    return (void *)(((_used_t *)node) + 1);
}

static inline void *_data_end(_used_t *node)
{
    return (void *)(((uint8_t *)_data(node)) + node->size);
}

static inline bool _in_data_range(_used_t *node, const void *ptr)
{
    return (ptr >= _data(node)) && (ptr < _data_end(node));
}

/**
 * @brief   Size with metadata of allocation
 */
static inline size_t _total_sz(uint16_t sz)
{
    return sizeof(_used_t) + sz;
}

/**
 * @brief   _used_t typed alias for _total_sz
 */
static inline size_t __total_sz(_used_t *node)
{
    return _total_sz(node->size);
}

/**
 * @brief   aligns @p size to the next word alignment.
 */
static inline size_t _al_sz(size_t size)
{
    if (size % _PKTBUF_ALIGN_BYTES) {
        return size + (_PKTBUF_ALIGN_BYTES - (size % _PKTBUF_ALIGN_BYTES));
    }
    else {
        return size;
    }
}

/**
 * @brief   aligned size with metadata
 */
static inline size_t __al_total_sz(_used_t *node)
{
    return _al_sz(__total_sz(node));
}

/**
 * @brief   Index of an allocation's first byte in buffer
 */
static inline unsigned int _start_idx(_used_t *node)
{
    return (int)(((uint8_t *)node) - _buf);
}

/**
 * @brief   Index of an allocation's last byte in buffer
 */
static inline unsigned int _end_idx(_used_t *node)
{
    return _start_idx(node) + __total_sz(node) - 1;
}

static _used_t *_find(_used_t **prev_ptr, _used_t **node_ptr, const void *ptr)
{
    _used_t *node = _head(), *prev = NULL;

    if (ptr != NULL) {
        while (node != NULL) {
            if (_in_data_range(node, ptr)) {
                *prev_ptr = prev;
                *node_ptr = node;
                return node;
            }

            prev = node;
            node = (_used_t *)node->next;
        }
    }

    *prev_ptr = NULL;
    *node_ptr = NULL;

    return NULL;
}

/**
 * @brief   Allocate chunk of @p size in _buf
 */
void *_pktbuf_internal_alloc(size_t size)
{
    _used_t *node = _head(), *old_next, *new_next;

    if ((size == 0) || (size > NG_PKTBUF_SIZE)) {
        return NULL;
    }

    if (node->size == 0) {          /* if head is currently not initialized */
        if (node->next == NULL || (_start_idx(node->next) >= _total_sz(size))) {
            /* if enough space is there */
            node->size = size;  /* just take it */
            node->pkts = 1;

            return _data(node);
        }
        else if (node->next != NULL) {
            /* else go directly to next allocation if it exists */
            node = node->next;
        }
    }

    while ((node->next != NULL) /* while not last chunk allocation */
           /* and if space between current and next allocation is not big enough */
           && ((_start_idx(node->next) - _end_idx(node)) < _al_sz(_total_sz(size)))) {
        node = node->next;
    }

    /* jump ahead size of current packet */
    new_next = (_used_t *)(((uint8_t *)node) + __al_total_sz(node));

    if ((((uint8_t *)new_next) + size) > (((uint8_t *)_head()) + NG_PKTBUF_SIZE)) {
        /* new packet does not fit into _pktbuf */
        return NULL;
    }

    old_next = node->next;
    node->next = new_next;
    node->next->next = old_next;

    node = new_next;
    node->size = size;
    node->pkts = 1;

#ifdef DEVELHELP

    if ((_end_idx(node) + 1) > _pktbuf_max_bytes) {
        _pktbuf_max_bytes = _end_idx(node) + 1;
    }

#endif

    return _data(node);
}

bool _pktbuf_internal_add_pkt(void *ptr)
{
    _used_t *prev, *node;

    if (_find(&prev, &node, ptr) != NULL) {
        (node->pkts)++;

        return true;
    }

    return false;
}

static inline void _free_helper(_used_t *prev, _used_t *node)
{
    if ((--(node->pkts)) == 0) {
        if (prev == NULL) {
            node->size = 0;
        }
        else {
            prev->next = node->next;
        }
    }
}

void _pktbuf_internal_free(void *ptr)
{
    _used_t *prev, *node;

    if (_find(&prev, &node, ptr) != NULL) {
        _free_helper(prev, node);
    }
}

void *_pktbuf_internal_realloc(void *ptr, size_t size)
{
    _used_t *new, *prev, *orig = NULL;

    if ((size == 0) || (size > NG_PKTBUF_SIZE)) {
        return NULL;
    }

    _find(&prev, &orig, ptr);

    if ((orig != NULL) &&
        ((orig->size >= size)       /* size in orig is sufficient */
         || ((orig->next == NULL)   /* or orig is last packet and buffer space is sufficient */
             && ((_start_idx(orig) + _total_sz(size)) < NG_PKTBUF_SIZE))
         || ((orig->next != NULL)   /* or space between orig and orig->next is sufficient */
             && ((_start_idx(orig->next) - _start_idx(orig)) >= _total_sz(size))))) {
        orig->size = size;

        return ptr;
    }

    new = _pktbuf_internal_alloc(size);

    if (new != NULL) {
        if (orig != NULL) {
            memcpy(_data(new), ptr, (orig->size < size) ? orig->size : size);
            _free_helper(prev, orig);
        }

        return _data(new);
    }

    return NULL;
}

bool _pktbuf_internal_contains(const void *ptr)
{
    return ((_buf < ((uint8_t *)ptr)) && (((uint8_t *)ptr) <= &(_buf[NG_PKTBUF_SIZE - 1])));
}

#ifdef DEVELHELP
void _pktbuf_internal_stats(void)
{
    _used_t *ptr = _head();

    printf("== Static packet buffer ==\n");
    printf("--- Maximum number of reserved bytes: %u ---\n", _pktbuf_max_bytes);
    while (ptr) {
        printf("**** %p: next: %p, size: %" PRIu16 ", pkts: %" PRIu8 " ****\n",
               (void *)ptr, (void *)ptr->next, ptr->size, ptr->pkts);
        od_hex_dump(_data(ptr), ptr->size, OD_WIDTH_DEFAULT);
        puts("");
        ptr = ptr->next;
    }
}
#endif

/* for testing */
#ifdef TEST_SUITES
bool _pktbuf_internal_is_empty(void)
{
    return ((_head()->next == NULL) && (_head()->size == 0));
}

void _pktbuf_internal_reset(void)
{
    _head()->next = NULL;
    _head()->size = 0;
#ifdef DEVELHELP
    _pktbuf_max_bytes = 0;
#endif
}
#endif  /* TEST_SUITES */
#endif  /* NG_PKTBUF_SIZE > 0 */

/** @} */
