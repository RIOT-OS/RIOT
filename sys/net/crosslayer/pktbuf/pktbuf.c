/*
 * Copyright (C) 2014 Martine Lenders <mlenders@inf.fu-berlin.de>
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
#include "pkt.h"
#include "pktbuf.h"

/**
 * @brief   Data type to represent describe chunks in packet buffer.
 */
typedef struct __attribute__((packed)) _pkt_internal_t {
    struct _pkt_internal_t *next;
    uint8_t processing;
    pkt_hlist_t *header_ptr;
    void *data;
    pktsize_t size;
    pkt_proto_t proto;
} _pkt_internal_t;

static uint8_t _pktbuf[PKTBUF_SIZE];
static mutex_t _pktbuf_mutex = MUTEX_INIT;

/**
 * @brief   Get first element in packet buffer.
 */
static inline _pkt_internal_t *_pktbuf_head(void)
{
    return (_pkt_internal_t *)(&(_pktbuf[0]));
}

/**
 * @brief   Get _pkt_internal from pkt_t
 *
 * @param[in] pkt   An external representation packet
 *
 * @return  Internal representation of a packet.
 */
static inline _pkt_internal_t *_pkt_internal_cast(pkt_t *pkt)
{
    return (_pkt_internal_t *)(((uint8_t *)pkt) - sizeof(_pkt_internal_t) + sizeof(pkt_t));
}

/**
 * @brief   Get pkt_t from _pkt_internal_t
 *
 * @param[in] pkt   An internal representation packet
 *
 * @return  External representation of a packet.
 */
static inline pkt_t *_pkt_cast(_pkt_internal_t *pkt)
{
    return (pkt_t *)(&(pkt->header_ptr));
}

/**
 * @brief   Calculates total size (@p size + size of _pkt_internal_t descriptive
 *          header) of a chunk in packet buffer.
 *
 * @param[in] size  A given packet size
 *
 * @return  Total size of a packet in memory.
 */
static inline pktsize_t _pkt_total_sz(pktsize_t size)
{
    return (pktsize_t)(sizeof(_pkt_internal_t) + size);
}

/**
 * @brief   Get pointer to the byte after the last byte of a packet.
 *
 * @param[in] pkt   A packet
 *
 * @return  Pointer to the byte after the last byte of a packet
 */
static inline void *_pkt_end(_pkt_internal_t *pkt)
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
static inline pktsize_t _pktbuf_start_idx(_pkt_internal_t *pkt)
{
    return (pktsize_t)(((uint8_t *)pkt) - (&(_pktbuf[0])));
}

/**
 * @brief   Get index in packet buffer of the last byte of a packet's data part
 *
 * @param[in] pkt    A packet
 *
 * @return  Index in packet buffer of the last byte of *pkt*'s data part.
 */
static inline pktsize_t _pktbuf_end_idx(_pkt_internal_t *pkt)
{
    return _pktbuf_start_idx(pkt) + _pkt_total_sz(pkt->size) - 1;
}

static _pkt_internal_t *_pktbuf_find_with_prev(_pkt_internal_t **prev_ptr,
        _pkt_internal_t **packet_ptr, const void *pkt)
{
    _pkt_internal_t *packet = _pktbuf_head(), *prev = NULL;

    while (packet != NULL) {

        if (packet->data == pkt) {
            *prev_ptr = prev;
            *packet_ptr = packet;
            return packet;
        }

        prev = packet;
        packet = packet->next;
    }

    *prev_ptr = NULL;
    *packet_ptr = NULL;

    return NULL;
}

static _pkt_internal_t *_pktbuf_find(const void *pkt)
{
    _pkt_internal_t *packet = _pktbuf_head();

#ifdef DEVELHELP

    if (pkt == NULL) {
        return NULL;
    }

#endif /* DEVELHELP */

    while (packet != NULL) {
        if ((((void *)packet) <= pkt) && (pkt < _pkt_end(packet))) {
            mutex_unlock(&_pktbuf_mutex);
            return packet;
        }

        packet = (_pkt_internal_t *)packet->next;
    }

    return NULL;
}

static _pkt_internal_t *_pktbuf_alloc(pktsize_t size)
{
    _pkt_internal_t *packet = _pktbuf_head(), *old_next;

    if ((size == 0) || (size > PKTBUF_SIZE)) {
        return NULL;
    }

    if ((packet->size == 0)         /* if first packet is currently not initialized
                                     * but next packet and no space between first
                                     * and next in its slot */
        && (packet->processing == 0)
        && (packet->next != NULL)
        && ((_pktbuf_start_idx(packet->next) - _pkt_total_sz(packet->size)) < size)) {
        packet = packet->next;
    }

    /* while packet is not initialized */
    while ((packet->processing > 0) || (packet->size > size)) {
        old_next = packet->next;

        /* if current packet is the last in buffer, but buffer space is exceeded */
        if ((old_next == NULL)
            && ((_pktbuf_end_idx(packet) + _pkt_total_sz(size)) > PKTBUF_SIZE)) {
            return NULL;
        }

        /* if current packet is the last in the buffer or if space between
         * current packet and next packet is big enough */
        if ((old_next == NULL)
            || ((_pktbuf_start_idx(packet->next) - _pktbuf_end_idx(packet)) >= _pkt_total_sz(size))) {

            _pkt_internal_t *new_next = (_pkt_internal_t *)(((uint8_t *)packet) + _pkt_total_sz(packet->size));
            /* jump ahead size of current packet. */
            packet->next = new_next;
            packet->next->next = old_next;

            packet = new_next;

            break;
        }

        packet = old_next;
    }

    packet->processing = 1;
    packet->size = size;
    packet->data = (void *)(packet + 1); /* first pointer after packet */
    packet->header_ptr = NULL;

    return packet;
}

static void _pktbuf_free(_pkt_internal_t *prev, _pkt_internal_t *packet)
{
    if ((packet->processing)-- > 1) {
        /* `> 1` because packet->processing may already be 0 in which case
         * --(packet->processing) would wrap to 255. */
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

pkt_t *pktbuf_alloc(pktsize_t payload_len)
{
    _pkt_internal_t *packet;

    mutex_lock(&_pktbuf_mutex);

    packet = _pktbuf_alloc(payload_len);

    if (packet == NULL) {
        mutex_unlock(&_pktbuf_mutex);
        return NULL;
    }

    mutex_unlock(&_pktbuf_mutex);

    return _pkt_cast(packet);
}

pkt_t *pktbuf_realloc_payload(const pkt_t *pkt, pktsize_t payload_len)
{
    _pkt_internal_t *new, *prev, *orig;

    mutex_lock(&_pktbuf_mutex);

    if ((payload_len == 0) || (payload_len > PKTBUF_SIZE)) {
        mutex_unlock(&_pktbuf_mutex);
        return NULL;
    }

    _pktbuf_find_with_prev(&prev, &orig, pkt->payload_data);

    if ((orig != NULL) &&
        ((orig->size >= payload_len)    /* and *orig* is last packet, and space in
                                         * _pktbuf is sufficient */
         || ((orig->next == NULL)
             && ((_pktbuf_start_idx(orig) + _pkt_total_sz(payload_len)) < PKTBUF_SIZE))
         || ((orig->next != NULL)   /* or space between pointer and the next is big enough: */
             && ((_pktbuf_start_idx(orig->next) - _pktbuf_start_idx(orig))
                 >= _pkt_total_sz(payload_len))))) {
        orig->size = payload_len;

        mutex_unlock(&_pktbuf_mutex);
        return (void *)pkt;
    }

    new = _pktbuf_alloc(payload_len);

    if (new == NULL) {
        mutex_unlock(&_pktbuf_mutex);
        return NULL;
    }

    /* cppcheck-suppress nullPointer cppcheck says this check is redundant but
     * it's * vital: orig is NULL if pkt was not previously allocated in the
     * packet buffer and thus it's data needs to be copied. */
    if (orig != NULL) {
        memcpy(new->data, orig->data, orig->size);
        new->header_ptr = orig->header_ptr;
        _pktbuf_free(prev, orig);
    }
    else {
        memcpy(new->data, pkt->payload_data, pkt->payload_len);
    }


    mutex_unlock(&_pktbuf_mutex);

    return _pkt_cast(new);
}

pkt_t *pktbuf_insert(const void *payload_data, pktsize_t payload_len)
{
    _pkt_internal_t *packet;

    if ((payload_data == NULL) || (payload_len == 0)) {
        return NULL;
    }

    mutex_lock(&_pktbuf_mutex);

    packet = _pktbuf_alloc(payload_len);

    if (packet == NULL) {
        mutex_unlock(&_pktbuf_mutex);
        return NULL;
    }

    memcpy(packet->data, payload_data, payload_len);

    mutex_unlock(&_pktbuf_mutex);
    return _pkt_cast(packet);
}

int pktbuf_copy(void *data, const void *data_new, pktsize_t data_new_len)
{
    _pkt_internal_t *packet;

#ifdef DEVELHELP

    if (data_new == NULL) {
        return -EFAULT;
    }

    if (data == NULL) {
        return -EINVAL;
    }

#endif /* DEVELHELP */

    mutex_lock(&_pktbuf_mutex);

    packet = _pktbuf_find(data);

    if ((packet != NULL) && (packet->size > 0) && (packet->processing > 0)) {
        /* if packet does not point to data's meta-data */
        if ((packet->data > data) || (_pkt_end(packet) <= data)) {
            return -EINVAL;
        }

        /* packet space not engough? */
        if (data_new_len > (pktsize_t)(((uint8_t *)_pkt_end(packet)) - ((uint8_t *)data))) {
            mutex_unlock(&_pktbuf_mutex);
            return -ENOMEM;
        }
    }

    memcpy(data, data_new, data_new_len);

    mutex_unlock(&_pktbuf_mutex);
    return data_new_len;
}

int pktbuf_add_header(pkt_t *pkt, void *header_data, pktsize_t header_len)
{
    _pkt_internal_t *packet, *header;

#ifdef DEVELHELP

    if (header_data == NULL) {
        return -EFAULT;
    }

    if (pkt == NULL) {
        return -EINVAL;
    }

#endif /* DEVELHELP */

    mutex_lock(&_pktbuf_mutex);

    packet = _pktbuf_find(pkt);

    if ((packet == NULL) || (_pkt_cast(packet) != pkt)) {
        mutex_unlock(&_pktbuf_mutex);
        return -EINVAL;
    }

    header = _pktbuf_alloc(header_len);

    if (header == NULL) {
        mutex_unlock(&_pktbuf_mutex);
        return -ENOMEM;
    }

    memcpy(header->data, header_data, header_len);

    pkt_add_header(_pkt_cast(packet), (pkt_hlist_t *)_pkt_cast(header));

    mutex_unlock(&_pktbuf_mutex);

    return 0;
}

static void _pkt_release(const pkt_t *pkt)
{
    _pkt_internal_t *packet = _pktbuf_head(), *prev = NULL;

    while (packet != NULL) {

        if ((_pkt_cast(packet) <= pkt) && (((void *)pkt) < _pkt_end(packet))) {
            _pktbuf_free(prev, packet);

            mutex_unlock(&_pktbuf_mutex);
            return;
        }

        prev = packet;
        packet = packet->next;
    }
}

void pktbuf_remove_header(pkt_t *pkt, pkt_hlist_t *header)
{
    mutex_lock(&_pktbuf_mutex);

    pkt_remove_header(pkt, header);
    _pkt_release((pkt_t *)header);

    mutex_unlock(&_pktbuf_mutex);
}

void pktbuf_hold(const pkt_t *pkt)
{
    _pkt_internal_t *packet;

    mutex_lock(&_pktbuf_mutex);

    packet = _pktbuf_find(pkt);

    if (packet != NULL) {
        _pkt_internal_t *ptr = packet;

        while (ptr) {
            ptr->processing++;

            if (ptr->header_ptr == NULL) {
                break;
            }

            ptr = _pkt_internal_cast((pkt_t *)(ptr->header_ptr));
        }
    }

    mutex_unlock(&_pktbuf_mutex);
}

void pktbuf_release(const pkt_t *pkt)
{
    _pkt_internal_t *ptr = _pkt_internal_cast((pkt_t *)pkt); /* cast to remove const */

    if (pkt == NULL || !pktbuf_contains_pkt(pkt)) {
        return;
    }

    mutex_lock(&_pktbuf_mutex);

    while (ptr != NULL) {
        _pkt_internal_t *next = NULL;

        if (ptr->header_ptr != NULL) {
            next = _pkt_internal_cast((pkt_t *)(ptr->header_ptr));
        }

        _pkt_release(_pkt_cast(ptr));

        ptr = next;
    }

    mutex_unlock(&_pktbuf_mutex);
}

#ifdef DEVELHELP
#include <stdio.h>

void pktbuf_print(void)
{
    _pkt_internal_t *packet = _pktbuf_head();
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
        packet = (_pkt_internal_t *)packet->next;
    }

    while (packet != NULL) {
        uint8_t *data = packet->data;

        printf("chunk %d (%p):\n", i, (void *)packet);
        printf("  next: %p\n", (void *)(packet->next));
        printf("  processing: %" PRIu8 "\n", packet->processing);
        printf("  size: %" PRIpktsize "\n", packet->size);
        printf("  data: (start address: %p)\n   ", packet->data);
        printf("  protocol: 0x%04x\n", packet->proto);
        printf("  header_ptr: %p", (void *)(packet->header_ptr));

        if (packet->next != NULL) {
            printf("  free data after: %" PRIu32 "\n",
                   (uint32_t)(_pktbuf_start_idx(packet->next) - _pktbuf_end_idx(packet) - 1));
        }
        else {
            printf("  free data after: %" PRIu32 "\n", (uint32_t)(PKTBUF_SIZE - _pktbuf_end_idx(packet) - 1));

        }

        if (packet->size > PKTBUF_SIZE) {
            printf(" We have a problem: packet->size (%" PRIu32 ") > PKTBUF_SIZE (%" PRIu32 ")\n",
                   (uint32_t)(packet->size), (uint32_t)PKTBUF_SIZE);
        }
        else {
            for (pktsize_t j = 0; j < packet->size; j++) {
                printf(" %02x", data[j]);

                if (((j + 1) % 16) == 0) {
                    printf("\n   ");
                }
            }

            printf("\n\n");
        }

        packet = (_pkt_internal_t *)packet->next;
        i++;
    }

    printf("===================================================\n");
    printf("\n");

    mutex_unlock(&_pktbuf_mutex);

}
#endif

#ifdef TEST_SUITES
pktsize_t pktbuf_bytes_allocated(void)
{
    _pkt_internal_t *packet = _pktbuf_head();
    pktsize_t bytes = 0;

    mutex_lock(&_pktbuf_mutex);

    while (packet != NULL) {
        bytes += packet->size;
        packet = packet->next;
    }

    mutex_unlock(&_pktbuf_mutex);

    return bytes;
}

unsigned int pktbuf_packets_allocated(void)
{
    _pkt_internal_t *packet = _pktbuf_head();
    unsigned int packets = 0;

    mutex_lock(&_pktbuf_mutex);

    while (packet != NULL) {
        if ((packet != _pktbuf_head()) || (packet->size > 0)) { /* ignore head if head->size == 0 */
            packets++;
        }

        packet = packet->next;
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
