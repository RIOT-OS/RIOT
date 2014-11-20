/*
 * Copyright (C) 2014 Martin Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 *
 * @file    pktbuf.c
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */

#include <errno.h>
#include <inttypes.h>
#include <stdlib.h>
#include <string.h>

#include "mutex.h"
#include "pktbuf.h"

#if PKTBUF_SIZE < 128
typedef uint8_t _pktsize_t;
#elif PKTBUF_SIZE < 65536
typedef uint16_t _pktsize_t;
#else
typedef size_t _pktsize_t;
#endif

typedef struct __attribute__((packed)) _packet_t {
    volatile struct _packet_t *next;
    uint8_t processing;
    _pktsize_t size;
} _packet_t;

static uint8_t _pktbuf[PKTBUF_SIZE];
static mutex_t _pktbuf_mutex = MUTEX_INIT;

/**
 * @brief   Get first element in packet buffer.
 */
static inline _packet_t *_pktbuf_head(void)
{
    return (_packet_t *)(&(_pktbuf[0]));
}

/**
 * @brief   Get data part (memory behind `_packet_t` descriptive header) of a packet
 *
 * @param[in] pkt   A packet
 *
 * @return  Data part of the packet.
 */
static inline void *_pkt_data(_packet_t *pkt)
{
    return (void *)(((_packet_t *)pkt) + 1);
}

/**
 * @brief   Calculates total size (*size* + size of `_packet_t` descriptive header) of a
 *          packet in memory
 *
 * @param[in] size  A given packet size
 *
 * @return  Total size of a packet in memory.
 */
static inline size_t _pkt_total_sz(size_t size)
{
    return sizeof(_packet_t) + size;
}

/**
 * @brief   Get pointer to the byte after the last byte of a packet.
 *
 * @param[in] pkt   A packet
 *
 * @return  Pointer to the last byte of a packet
 */
static inline void *_pkt_end(_packet_t *pkt)
{
    return (void *)((uint8_t *)pkt + _pkt_total_sz(pkt->size));
}

/**
 * @brief   Get index in packet buffer of the first byte of a packet's data part
 *
 * @param[in] pkt    A packet
 *
 * @return  Index in packet buffer of the first byte of *pkt*'s data part.
 */
static inline size_t _pktbuf_start_idx(_packet_t *pkt)
{
    return (size_t)(((uint8_t *)pkt) - (&(_pktbuf[0])));
}

/**
 * @brief   Get index in packet buffer of the last byte of a packet's data part
 *
 * @param[in] pkt    A packet
 *
 * @return  Index in packet buffer of the last byte of *pkt*'s data part.
 */
static inline size_t _pktbuf_end_idx(_packet_t *pkt)
{
    return _pktbuf_start_idx(pkt) + _pkt_total_sz(pkt->size) - 1;
}

static _packet_t *_pktbuf_find_with_prev(_packet_t **prev_ptr,
        _packet_t **packet_ptr, const void *pkt)
{
    _packet_t *packet = _pktbuf_head(), *prev = NULL;

    while (packet != NULL) {

        if (_pkt_data(packet) == pkt) {
            *prev_ptr = prev;
            *packet_ptr = packet;
            return packet;
        }

        prev = packet;
        packet = (_packet_t *)packet->next;
    }

    *prev_ptr = NULL;
    *packet_ptr = NULL;

    return NULL;
}

static _packet_t *_pktbuf_find(const void *pkt)
{
    _packet_t *packet = _pktbuf_head();

#ifdef DEVELHELP

    if (pkt == NULL) {
        return NULL;
    }

#endif /* DEVELHELP */

    while (packet != NULL) {
        if ((_pkt_data(packet) <= pkt) && (pkt < _pkt_end(packet))) {
            mutex_unlock(&_pktbuf_mutex);
            return packet;
        }

        packet = (_packet_t *)packet->next;
    }

    return NULL;
}

static _packet_t *_pktbuf_alloc(size_t size)
{
    _packet_t *packet = _pktbuf_head(), *old_next;

    if ((size == 0) || (size > PKTBUF_SIZE)) {
        return NULL;
    }

    if ((packet->size == 0)         /* if first packet is currently not initialized
                                     * but next packet and no space between first
                                     * and next in its slot */
        && (packet->processing == 0)
        && (packet->next != NULL)
        && ((_pktbuf_start_idx((_packet_t *)(packet->next)) - _pkt_total_sz(packet->size)) < size)) {
        packet = (_packet_t *)packet->next;
    }

    /* while packet is not initialized */
    while ((packet->processing > 0) || (packet->size > size)) {
        old_next = (_packet_t *)packet->next;

        /* if current packet is the last in buffer, but buffer space is exceeded */
        if ((old_next == NULL)
            && ((_pktbuf_end_idx(packet) + _pkt_total_sz(size)) > PKTBUF_SIZE)) {
            return NULL;
        }

        /* if current packet is the last in the buffer or if space between
         * current packet and next packet is big enough */
        if ((old_next == NULL)
            || ((_pktbuf_start_idx((_packet_t *)(packet->next)) - _pktbuf_end_idx(packet)) >= _pkt_total_sz(
                    size))) {

            _packet_t *new_next = (_packet_t *)(((uint8_t *)packet) + _pkt_total_sz(packet->size));
            /* jump ahead size of current packet. */
            packet->next = new_next;
            packet->next->next = old_next;

            packet = new_next;

            break;
        }

        packet = old_next;
    }

    packet->size = size;
    packet->processing = 1;

    return packet;
}

static void _pktbuf_free(_packet_t *prev, _packet_t *packet)
{
    if ((packet->processing)-- > 1) { /* `> 1` because packet->processing may already
                                       * be 0 in which case --(packet->processing)
                                       * would wrap to 255. */
        return;
    }

    if (prev == NULL) {     /* packet is _pktbuf_head() */
        packet->size = 0;
    }
    else {
        prev->next = packet->next;
    }
}

int pktbuf_contains(const void *pkt)
{
    return ((&(_pktbuf[0]) < ((uint8_t *)pkt)) && (((uint8_t *)pkt) <= &(_pktbuf[PKTBUF_SIZE - 1])));
}

void *pktbuf_alloc(size_t size)
{
    _packet_t *packet;

    mutex_lock(&_pktbuf_mutex);

    packet = _pktbuf_alloc(size);

    if (packet == NULL) {
        mutex_unlock(&_pktbuf_mutex);
        return NULL;
    }

    mutex_unlock(&_pktbuf_mutex);

    return _pkt_data(packet);
}

void *pktbuf_realloc(const void *pkt, size_t size)
{
    _packet_t *new, *prev, *orig;

    mutex_lock(&_pktbuf_mutex);

    if ((size == 0) || (size > PKTBUF_SIZE)) {
        mutex_unlock(&_pktbuf_mutex);
        return NULL;
    }

    _pktbuf_find_with_prev(&prev, &orig, pkt);

    if ((orig != NULL) &&
        ((orig->size >= size)       /* and *orig* is last packet, and space in
                                     * _pktbuf is sufficient */
         || ((orig->next == NULL)
             && ((_pktbuf_start_idx(orig) + _pkt_total_sz(size)) < PKTBUF_SIZE))
         || ((orig->next != NULL)   /* or space between pointer and the next is big enough: */
             && ((_pktbuf_start_idx((_packet_t *)(orig->next)) - _pktbuf_start_idx(orig))
                 >= _pkt_total_sz(size))))) {
        orig->size = size;

        mutex_unlock(&_pktbuf_mutex);
        return (void *)pkt;
    }

    new = _pktbuf_alloc(size);

    if (new == NULL) {
        mutex_unlock(&_pktbuf_mutex);
        return NULL;
    }

    if (orig != NULL) {
        memcpy(_pkt_data(new), _pkt_data(orig), orig->size);

        _pktbuf_free(prev, orig);
    }
    else {
        memcpy(_pkt_data(new), pkt, size);
    }


    mutex_unlock(&_pktbuf_mutex);

    return _pkt_data(new);
}

void *pktbuf_insert(const void *data, size_t size)
{
    _packet_t *packet;

    if ((data == NULL) || (size == 0)) {
        return NULL;
    }

    mutex_lock(&_pktbuf_mutex);
    packet = _pktbuf_alloc(size);

    if (packet == NULL) {
        mutex_unlock(&_pktbuf_mutex);
        return NULL;
    }

    memcpy(_pkt_data(packet), data, size);

    mutex_unlock(&_pktbuf_mutex);
    return _pkt_data(packet);
}

int pktbuf_copy(void *pkt, const void *data, size_t data_len)
{
    _packet_t *packet;

    mutex_lock(&_pktbuf_mutex);

#ifdef DEVELHELP

    if (data == NULL) {
        mutex_unlock(&_pktbuf_mutex);
        return -EFAULT;
    }

    if (pkt == NULL) {
        mutex_unlock(&_pktbuf_mutex);
        return -EINVAL;
    }

#endif /* DEVELHELP */

    packet = _pktbuf_find(pkt);


    if ((packet != NULL) && (packet->size > 0) && (packet->processing > 0)) {
        /* packet space not engough? */
        if (data_len > (size_t)(((uint8_t *)_pkt_end(packet)) - ((uint8_t *)pkt))) {
            mutex_unlock(&_pktbuf_mutex);
            return -ENOMEM;
        }
    }


    memcpy(pkt, data, data_len);

    mutex_unlock(&_pktbuf_mutex);
    return data_len;
}

void pktbuf_hold(const void *pkt)
{
    _packet_t *packet;

    mutex_lock(&_pktbuf_mutex);

    packet = _pktbuf_find(pkt);

    if (packet != NULL) {
        packet->processing++;
    }

    mutex_unlock(&_pktbuf_mutex);
}

void pktbuf_release(const void *pkt)
{
    _packet_t *packet = _pktbuf_head(), *prev = NULL;

    mutex_lock(&_pktbuf_mutex);

    while (packet != NULL) {

        if ((_pkt_data(packet) <= pkt) && (pkt < _pkt_end(packet))) {
            _pktbuf_free(prev, packet);

            mutex_unlock(&_pktbuf_mutex);
            return;
        }

        prev = packet;
        packet = (_packet_t *)packet->next;
    }

    mutex_unlock(&_pktbuf_mutex);
}

#ifdef DEVELHELP
#include <stdio.h>

void pktbuf_print(void)
{
    _packet_t *packet = _pktbuf_head();
    int i = 0;

    mutex_lock(&_pktbuf_mutex);

    printf("current pktbuf allocations:\n");
    printf("===================================================\n");

    if (packet->next == NULL && packet->size == 0) {
        printf("empty\n");
        printf("===================================================\n");
        printf("\n");
        mutex_unlock(&_pktbuf_mutex);

        return;
    }
    else if (packet->next != NULL && packet->size == 0) {
        packet = (_packet_t *)packet->next;
    }

    while (packet != NULL) {
        uint8_t *data = (uint8_t *)_pkt_data(packet);

        printf("packet %d (%p):\n", i, (void *)packet);
        printf("  next: %p\n", (void *)(packet->next));
        printf("  size: %" PRIu32 "\n", (uint32_t)packet->size);
        printf("  processing: %" PRIu8 "\n", packet->processing);

        if (packet->next != NULL) {
            printf("  free data after: %" PRIu32 "\n",
                   (uint32_t)(_pktbuf_start_idx((_packet_t *)(packet->next)) - _pktbuf_end_idx(packet) - 1));
        }
        else {
            printf("  free data after: %" PRIu32 "\n", (uint32_t)(PKTBUF_SIZE - _pktbuf_end_idx(packet) - 1));

        }

        printf("  data: (start address: %p)\n   ", data);

        if (packet->size > PKTBUF_SIZE) {
            printf(" We have a problem: packet->size (%" PRIu32 ") > PKTBUF_SIZE (%" PRIu32 ")\n",
                   (uint32_t)(packet->size), (uint32_t)PKTBUF_SIZE);
        }
        else {
            for (size_t j = 0; j < packet->size; j++) {
                printf(" %02x", data[j]);

                if (((j + 1) % 16) == 0) {
                    printf("\n   ");
                }
            }

            printf("\n\n");
        }

        packet = (_packet_t *)packet->next;
        i++;
    }

    printf("===================================================\n");
    printf("\n");

    mutex_unlock(&_pktbuf_mutex);

}
#endif

#ifdef TEST_SUITES
size_t pktbuf_bytes_allocated(void)
{
    _packet_t *packet = _pktbuf_head();
    size_t bytes = 0;

    mutex_lock(&_pktbuf_mutex);

    while (packet != NULL) {
        bytes += packet->size;
        packet = (_packet_t *)(packet->next);
    }

    mutex_unlock(&_pktbuf_mutex);

    return bytes;
}

unsigned int pktbuf_packets_allocated(void)
{
    _packet_t *packet = _pktbuf_head();
    unsigned int packets = 0;

    mutex_lock(&_pktbuf_mutex);

    while (packet != NULL) {
        if ((packet != _pktbuf_head()) || (packet->size > 0)) { /* ignore head if head->size == 0 */
            packets++;
        }

        packet = (_packet_t *)(packet->next);
    }

    mutex_unlock(&_pktbuf_mutex);

    return packets;
}

int pktbuf_is_empty(void)
{
    return ((_pktbuf_head()->next == NULL) && (_pktbuf_head()->size == 0));
}

void pktbuf_reset(void)
{
    memset(_pktbuf, 0, PKTBUF_SIZE);
    mutex_init(&_pktbuf_mutex);
}
#endif

/** @} */
