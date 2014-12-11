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

#include <stdbool.h>
#include <stdlib.h>

#include "_pktbuf_internal.h"
#include "net/ng_pktbuf.h"
#include "utlist.h"

#if NG_PKTBUF_SIZE == 0
/* chunk table to allow for free(ptr + x)-like behaviour */
typedef struct __attribute__((packed)) _chunk_list_t {
    struct _chunk_list_t *next;
    uint8_t *ptr;
} _chunk_list_t;

typedef struct __attribute__((packed)) _chunk_table_t {
    struct _chunk_table_t *next;
    uint8_t *range_start;
    size_t range_len;
    _chunk_list_t *chunks;
    uint8_t used;
} _chunk_table_t;

static _chunk_table_t *_chunk_table = NULL;

/* this organizes chunks, since free(ptr + x) is not possible on most platforms */
static _chunk_table_t *_create_table_entry(void *pkt, size_t size);
static _chunk_table_t *_find_chunk(const uint8_t *chunk, _chunk_table_t **prev,
                                   _chunk_list_t **node_res);
static inline bool _in_range(_chunk_table_t *entry, uint8_t *ptr);

void *_pktbuf_internal_alloc(size_t size)
{
    _chunk_table_t *entry;
    void *data;

    if (size == 0) {
        return 0;
    }

    data = malloc(size);

    if (data == NULL) {
        return NULL;
    }

    entry = _create_table_entry(data, size);

    if (entry == NULL) {
        free(data);

        return NULL;
    }

    /* cppcheck-suppress memleak entry will be freed eventually in _pktbuf_internal_free().
     * Checked with valgrind. */
    return data;
}

void *_pktbuf_internal_realloc(void *ptr, size_t size)
{
    _chunk_list_t *node = NULL;
    void *new = NULL;
    _chunk_table_t *entry;

    if (size == 0) {
        return NULL;
    }

    entry = _find_chunk(ptr, NULL, &node);

    /* entry can't be NULL since prelimanary _pktbuf_internal_contains() check ensures that */
    if ((ptr == entry->range_start) && (entry->chunks == NULL)) {
        new = realloc(entry->range_start, size);

        if (new == NULL) {
            return NULL;
        }

        entry->range_start = new;
        entry->range_len = size;
    }
    else {
        size_t range_len = entry->range_len;

        if (node != NULL) {
            range_len -= (node->ptr - entry->range_start);
        }

        new = malloc(size);

        if (new == NULL) {
            return NULL;
        }

        entry = _create_table_entry(new, size);

        if (entry == NULL) {
            free(new);

            return NULL;
        }

        memcpy(new, ptr, (size < range_len) ? size : range_len);
        _pktbuf_internal_free(ptr);
    }

    return new;
    /* cppcheck-suppress memleak entry will be freed eventually in _pktbuf_internal_free().
     * Checked with valgrind. */
}

bool _pktbuf_internal_add_pkt(void *ptr)
{
    _chunk_table_t *entry = _chunk_table;

    while (entry != NULL) {
        if (_in_range(entry, ptr)) {
            _chunk_list_t *node = malloc(sizeof(_chunk_list_t));

            if (node == NULL) {
                return false;
            }

            node->ptr = ptr;
            LL_PREPEND(entry->chunks, node);

            return true;
        }

        entry = entry->next;
    }

    return false;
}

void _pktbuf_internal_free(void *ptr)
{
    _chunk_list_t *node = NULL;
    _chunk_table_t *prev = NULL, *entry = _find_chunk(ptr, &prev, &node);

    if (node != NULL) {
        LL_DELETE(entry->chunks, node);
        free(node);
    }
    else if (entry->range_start == ptr) {
        entry->used = 0;
    }

    if (entry->chunks == NULL && entry->used == 0) {
        if (prev == NULL) {
            if (entry->next == NULL) {
                _chunk_table = NULL;
            }
            else {
                _chunk_table = entry->next;
            }
        }
        else {
            prev->next = entry->next;
        }

        free(entry->range_start);
        free(entry);
    }
}

bool _pktbuf_internal_contains(const void *ptr)
{
    return (_find_chunk(ptr, NULL, NULL) != NULL);
}

#ifdef DEVELHELP
void _pktbuf_internal_stats(void)
{
    printf("Dynamic packet buffer\n");
}
#endif

#ifdef TEST_SUITES
bool _pktbuf_internal_is_empty(void)
{
    return (_chunk_table == NULL);
}

void _pktbuf_internal_reset(void)
{
    _chunk_table_t *entry = _chunk_table;

    while (entry != NULL) {
        _chunk_table_t *next = entry->next;
        _chunk_list_t *node = entry->chunks;
        free(entry->range_start);

        while (entry->chunks != NULL) {
            LL_DELETE(entry->chunks, node);
            free(node);
        }

        free(entry);
        entry = next;
    }

    _chunk_table = NULL;
}
#endif

static _chunk_table_t *_create_table_entry(void *data, size_t size)
{
    _chunk_table_t *entry = (_chunk_table_t *)malloc(sizeof(_chunk_table_t));

    if (entry == NULL) {
        return NULL;
    }

    if (_chunk_table == NULL) {
        entry->next = NULL;
        _chunk_table = entry;
    }
    else {
        entry->next = _chunk_table;
        _chunk_table = entry;
    }

    entry->range_start = data;
    entry->range_len = size;
    entry->chunks = NULL;
    entry->used = 1;

    return entry;
}

static _chunk_table_t *_find_chunk(const uint8_t *chunk, _chunk_table_t **prev,
                                   _chunk_list_t **node_res)
{
    _chunk_table_t *entry = _chunk_table;

    if (prev != NULL) {
        *prev = NULL;
    }

    while (entry != NULL) {
        _chunk_list_t *node = entry->chunks;

        if (entry->range_start == chunk) {
            if (node_res != NULL) {
                *node_res = NULL;
            }

            return entry;
        }

        while (node != NULL) {
            if (node->ptr == chunk) {
                if (node_res != NULL) {
                    *node_res = node;
                }

                return entry;
            }

            node = node->next;
        }

        if (prev != NULL) {
            *prev = entry;
        }

        entry = entry->next;
    }

    return NULL;
}

static inline bool _in_range(_chunk_table_t *entry, uint8_t *ptr)
{
    return (entry != NULL) &&
           (ptr >= entry->range_start) &&
           (ptr < (entry->range_start + entry->range_len));
}
#endif

/** @} */
