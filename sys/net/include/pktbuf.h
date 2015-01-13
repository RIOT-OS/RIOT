/*
 * Copyright (C) 2014 Martin Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    pktbuf   Packet buffer
 * @ingroup     net
 * @brief       A global network packet buffer.
 * @{
 *
 * @file    pktbuf.h
 * @brief   Interface definition for the global network buffer. Network devices
 *          and layers can allocate space for packets here.
 *
 * @note    **WARNING!!** Do not store data structures that are not packed
 *          (defined with `__attribute__((packed))`) or enforce alignment in
 *          in any way in here. On some RISC architectures this *will* lead to
 *          alignment problems and can potentially result in segmentation/hard
 *          faults and other unexpected behaviour.
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef __PKTBUF_H_
#define __PKTBUF_H_

#include <stdlib.h>

#include "cpu-conf.h"
#include "pkt.h"

#ifdef __cplusplus
extern "C" {
#endif

#ifndef PKTBUF_SIZE
/**
 * @brief   Maximum size of the packet buffer.
 *
 * @details The rational here is to have at least space for 4 full-MTU IPv6
 *          packages (2 incoming, 2 outgoing; 2 * 2 * 1280 B = 5 KiB) +
 *          Meta-Data (roughly estimated to 1 KiB; might be smaller)
 */
#define PKTBUF_SIZE  (6144)
#endif  /* PKTBUF_SIZE */

/**
 * @brief   Allocates new packet data in the packet buffer. This also marks the
 *          allocated data as processed.
 *
 * @see @ref pktbuf_hold()
 *
 * @param[in] payload_len   The length of the packet you want to allocate.
 *
 * @return  Pointer to the packet in the packet buffer, on success.
 * @return  NULL, if no space is left in the packet buffer or size was 0.
 */
pkt_t *pktbuf_alloc(pktsize_t payload_len);

/**
 * @brief   Reallocates payload of packet in the packet buffer, without changing
 *          the content.
 *
 * @details If enough memory is available behind it or @p size is smaller than
 *          the original size the packet and payload will not be moved.
 *          Otherwise, it will be moved. If no space is available nothing
 *          happens. If @p pkt is not in the packet buffer it will be copied
 *          into it with pkt_t::payload_len == size.
 *
 * @param[in] pkt           A packet.
 * @param[in] payload_len   The new payload length for @p pkt.
 *
 * @return  New position of @p pkt, on success.
 * @return  NULL, if no space is left in the packet buffer or size was 0.
 *          The payload will remain where it was.
 */
pkt_t *pktbuf_realloc_payload(const pkt_t *pkt, pktsize_t payload_len);

/**
 * @brief   Allocates new packet in the packet buffer and copies data into it.
 *          This also marks the allocated data as processed for the current
 *          thread.
 *
 * @see @ref pktbuf_hold()
 *
 * @param[in] payload_data  Payload data you want to copy into the new packet.
 * @param[in] payload_len   The length of the packet you want to allocate.
 *
 * @return  Pointer to the packet in the packet buffer, on success.
 * @return  NULL, if no space is left in the packet buffer.
 */
pkt_t *pktbuf_insert(const void *payload_data, pktsize_t payload_len);

/**
 * @brief   Copies packet payload or header data into the packet buffer, safely.
 *
 * @details Use this instead of memcpy, since it is thread-safe and checks if
 *          @p data is
 *
 *          -# in the buffer at all
 *          -# its @p new_data is smaller or equal to the data allocated at
 *             @p new_data
 *
 *          It also uses the thread-safe abilities of the packet buffer.
 *
 *          If the @p data is not in the buffer the data is just copied as
 *          memcpy would do.
 *
 * @param[in,out] data      Some payload or header data pointer in the packet
 *                          buffer.
 * @param[in] new_data      The data you want to copy into the packet.
 * @param[in] new_data_len  The length of the data you want to copy.
 *
 * @return  @p new_data_len, on success.
 * @return  -EFAULT, if @p data is NULL and DEVELHELP is defined.
 * @return  -EINVAL, if @p data is NULL and DEVELHELP is defined.
 * @return  -EINVAL, if @p data is not a pointer to payload data in packet buffer.
 * @return  -ENOMEM, if @p data_len was greater than the packet size of @p data.
 */
int pktbuf_copy(void *data, const void *new_data, pktsize_t new_data_len);

/**
 * @brief   Adds a header to the packet and allocates it in packet buffer.
 *
 * @param[in] pkt           The packet to add the new header to
 * @param[in] header_data   The data of the header
 * @param[in] header_len    The length of the header
 *
 * @return  0, on success
 * @return  -EFAULT, if @p header_data is NULL and DEVELHELP is defined.
 * @return  -EINVAL, if @p pkt is NULL and DEVELHELP is defined.
 * @return  -EINVAL, if @p pkt not a packet in packet buffer.
 * @return  -ENOMEM, if no space is left in the packet buffer or @p header_len was 0.
 */
int pktbuf_add_header(pkt_t *pkt, void *header_data, pktsize_t header_len);

/**
 * @brief   Removes a header from the packet and releases it in the packet
 *          buffer.
 *
 * @param[in] pkt           The packet to add the new header to
 * @param[in] header        The header to remove.
 */
void pktbuf_remove_header(pkt_t *pkt, pkt_hlist_t *header);

/**
 * @brief   Marks the data as being processed.
 *
 * @details Internally this increments just a counter on the data.
 *          @ref pktbuf_release() decrements it. If the counter is <=0 the
 *          reserved data block in the buffer will be made available again.
 *
 * @param[in] pkt   The packet you want mark as being processed.
 */
void pktbuf_hold(const pkt_t *pkt);

/**
 * @brief   Marks the data as not being processed.
 *
 * @param[in] pkt   The packet you want mark as not being processed anymore.
 *
 * @details Internally this decrements just a counter on the data.
 *          @ref pktbuf_hold() increments and any allocation
 *          operation initializes it. If the counter is <=0 the reserved data
 *          block in the buffer will be made available again.
 */
void pktbuf_release(const pkt_t *pkt);

/**
 * @brief   Checks if a given pointer is stored in the packet buffer
 *
 * @param[in] ptr   Pointer to be checked
 *
 * @return  1, if @p ptr is in packet buffer
 * @return  0, otherwise
 */
int pktbuf_contains(const void *ptr);

/**
 * @brief   Checks if a given packet is stored in the packet buffer
 *
 * @param[in] pkt   Packet to be checked
 *
 * @return  1, if @p pkt is in packet buffer
 * @return  0, otherwise
 */
static inline int pktbuf_contains_pkt(const pkt_t *pkt)
{
    return pktbuf_contains(pkt);
}

/**
 * @brief   Prints current packet buffer to stdout if DEVELHELP is defined.
 */
#ifdef DEVELHELP
void pktbuf_print(void);
#else
#define pktbuf_print()  ;
#endif

/* for testing */
#ifdef TEST_SUITES
/**
 * @brief   Counts the number of allocated bytes
 *
 * @return  Number of allocated bytes
 */
pktsize_t pktbuf_bytes_allocated(void);

/**
 * @brief   Counts the number of allocated packets
 *
 * @return  Number of allocated packets
 */
unsigned int pktbuf_packets_allocated(void);

/**
 * @brief   Checks if packet buffer is empty
 *
 * @return  1, if packet buffer is empty
 * @return  0, if packet buffer is not empty
 */
int pktbuf_is_empty(void);

/**
 * @brief   Sets the whole packet buffer to 0
 */
void pktbuf_reset(void);
#endif

#ifdef __cplusplus
}
#endif

#endif /* __PKTBUF_H_ */
/** @} */
