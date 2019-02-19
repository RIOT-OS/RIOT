/*
 * Copyright (C) 2014 Martine Lenders <mlenders@inf.fu-berlin.de>
 *               2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_gnrc_pktbuf   Packet buffer
 * @ingroup     net_gnrc
 * @brief       A global network packet buffer.
 *
 * @note    **WARNING!!** Do not store data structures that are not packed
 *          (defined with `__attribute__((packed))`) or enforce alignment in
 *          in any way in here if @ref GNRC_PKTBUF_SIZE > 0. On some RISC architectures
 *          this *will* lead to alignment problems and can potentially result
 *          in segmentation/hard faults and other unexpected behaviour.
 *
 * @{
 *
 * @file
 * @brief   Interface definition for the global network buffer. Network devices
 *          and layers can allocate space for packets here.
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 * @author  Hauke Petersen <hauke.petersen@fu-berlin.de>
 */
#ifndef NET_GNRC_PKTBUF_H
#define NET_GNRC_PKTBUF_H

#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "cpu_conf.h"
#include "net/gnrc/pkt.h"
#include "net/gnrc/neterr.h"
#include "net/gnrc/nettype.h"
#include "utlist.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @def     GNRC_PKTBUF_SIZE
 * @brief   Maximum size of the static packet buffer.
 *
 * @details The rational here is to have at least space for 4 full-MTU IPv6
 *          packages (2 incoming, 2 outgoing; 2 * 2 * 1280 B = 5 KiB) +
 *          Meta-Data (roughly estimated to 1 KiB; might be smaller). If
 *          @ref GNRC_PKTBUF_SIZE is 0 the packet buffer will use dynamic memory
 *          management to allocate packets.
 */
#ifndef GNRC_PKTBUF_SIZE
#define GNRC_PKTBUF_SIZE    (6144)
#endif  /* GNRC_PKTBUF_SIZE */

/**
 * @brief   Initializes packet buffer module.
 */
void gnrc_pktbuf_init(void);

/**
 * @brief   Adds a new gnrc_pktsnip_t and its packet to the packet buffer.
 *
 * @warning **Do not** change the fields of the gnrc_pktsnip_t created by this
 *          function externally. This will most likely create memory leaks or
 *          not allowed memory access.
 *
 * @pre size < GNRC_PKTBUF_SIZE
 *
 * @param[in] next      Next gnrc_pktsnip_t in the packet. Leave NULL if you
 *                      want to create a new packet.
 * @param[in] data      Data of the new gnrc_pktsnip_t. If @p data is NULL no data
 *                      will be inserted into `result`.
 * @param[in] size      Length of @p data. If this value is 0 the
 *                      gnrc_pktsnip::data field of the newly created snip will
 *                      be NULL.
 * @param[in] type      Protocol type of the gnrc_pktsnip_t.
 *
 * @return  Pointer to the packet part that represents the new gnrc_pktsnip_t.
 * @return  NULL, if no space is left in the packet buffer.
 */
gnrc_pktsnip_t *gnrc_pktbuf_add(gnrc_pktsnip_t *next, const void *data, size_t size,
                                gnrc_nettype_t type);

/**
 * @brief   Marks the first @p size bytes in a received packet with a new
 *          packet snip that is appended to the packet.
 *
 * Graphically this can be represented as follows:
 *
 * ~~~~~~~~~~~~~~~~~~~
 * Before                                    After
 * ======                                    =====
 *                                                       (next)
 *  pkt->data                                 result->data <== pkt->data
 *  v                                         v                v
 * +--------------------------------+        +----------------+---------------+
 * +--------------------------------+        +----------------+---------------+
 *  \__________pkt->size___________/          \_result->size_/ \__pkt->size__/
 * ~~~~~~~~~~~~~~~~~~~
 *
 * If `size == pkt->size` then the resulting snip will point to NULL in its
 * gnrc_pktsnip_t::data field and its gnrc_pktsnip_t::size field will be 0.
 *
 * @pre @p pkt != NULL && @p size != 0
 *
 * @param[in] pkt   A received packet.
 * @param[in] size  The size of the new packet snip.
 * @param[in] type  The type of the new packet snip.
 *
 * @note    It's not guaranteeed that `result->data` points to the same address
 *          as the original `pkt->data`.
 *
 * @return  The new packet snip in @p pkt on success.
 * @return  NULL, if pkt == NULL or size == 0 or size > pkt->size or pkt->data == NULL.
 * @return  NULL, if no space is left in the packet buffer.
 */
gnrc_pktsnip_t *gnrc_pktbuf_mark(gnrc_pktsnip_t *pkt, size_t size, gnrc_nettype_t type);

/**
 * @brief   Reallocates gnrc_pktsnip_t::data of @p pkt in the packet buffer, without
 *          changing the content.
 *
 * @pre `pkt != NULL`
 * @pre `(pkt->size > 0) <=> (pkt->data != NULL)`
 * @pre gnrc_pktsnip_t::data of @p pkt is in the packet buffer if it is not NULL.
 *
 * @details If enough memory is available behind it or @p size is smaller than
 *          the original size of the packet then gnrc_pktsnip_t::data of @p pkt will
 *          not be moved. Otherwise, it will be moved. If no space is available
 *          nothing happens.
 *
 * @param[in] pkt   A packet part.
 * @param[in] size  The size for @p pkt.
 *
 * @return  0, on success
 * @return  ENOMEM, if no space is left in the packet buffer.
 */
int gnrc_pktbuf_realloc_data(gnrc_pktsnip_t *pkt, size_t size);

/**
 * @brief   Increases gnrc_pktsnip_t::users of @p pkt atomically.
 *
 * @param[in] pkt   A packet.
 * @param[in] num   Number you want to increment gnrc_pktsnip_t::users of @p pkt by.
 */
void gnrc_pktbuf_hold(gnrc_pktsnip_t *pkt, unsigned int num);

/**
 * @brief   Decreases gnrc_pktsnip_t::users of @p pkt atomically and removes it if it
 *          reaches 0 and reports a possible error through an error code, if
 *          @ref net_gnrc_neterr is included.
 *
 * @pre All snips of @p pkt must be in the packet buffer.
 *
 * @param[in] pkt   A packet.
 * @param[in] err   An error code.
 */
void gnrc_pktbuf_release_error(gnrc_pktsnip_t *pkt, uint32_t err);

/**
 * @brief   Decreases gnrc_pktsnip_t::users of @p pkt atomically and removes it if it
 *          reaches 0 and reports @ref GNRC_NETERR_SUCCESS.
 *
 * @param[in] pkt   A packet.
 */
static inline void gnrc_pktbuf_release(gnrc_pktsnip_t *pkt)
{
    gnrc_pktbuf_release_error(pkt, GNRC_NETERR_SUCCESS);
}

/**
 * @brief   Must be called once before there is a write operation on a
 *          [packet snip](@ref gnrc_pktsnip_t) in a thread.
 *
 * @details This function duplicates a packet snip in the packet buffer (both
 *          the instance of the gnrc_pktsnip_t and its data) if
 *          gnrc_pktsnip_t::users of @p pkt > 1.
 *
 * @param[in] pkt   The packet snip you want to write into.
 *
 * @return  The (new) pointer to the packet snip.
 * @return  NULL, if gnrc_pktsnip_t::users of @p pkt > 1 and if there is not
 *          enough space in the packet buffer.
 */
gnrc_pktsnip_t *gnrc_pktbuf_start_write(gnrc_pktsnip_t *pkt);

/**
 * @brief   Create a IOVEC representation of the packet pointed to by *pkt*
 *
 * @pre `(len != NULL)`
 *
 * @details This function will create a new packet snip in the packet buffer,
 *          which points to the given *pkt* and contains a IOVEC representation
 *          of the referenced packet in its data section.
 *
 * @param[in]  pkt  Packet to export as IOVEC
 * @param[out] len  Number of elements in the IOVEC
 *
 * @return  Pointer to the 'IOVEC packet snip'
 * @return  NULL, if packet is empty of the packet buffer is full
 */
gnrc_pktsnip_t *gnrc_pktbuf_get_iovec(gnrc_pktsnip_t *pkt, size_t *len);

/**
 * @brief   Deletes a snip from a packet and the packet buffer.
 *
 * @param[in] pkt   A packet.
 * @param[in] snip  A snip in the packet.
 *
 * @return  The new reference to @p pkt.
 */
gnrc_pktsnip_t *gnrc_pktbuf_remove_snip(gnrc_pktsnip_t *pkt, gnrc_pktsnip_t *snip);

/**
 * @brief   Replace a snip from a packet and the packet buffer by another snip.
 *
 * @param[in] pkt   A packet
 * @param[in] old   snip currently in the packet
 * @param[in] add   snip which will replace old
 *
 * @return  The new reference to @p pkt
 */
gnrc_pktsnip_t *gnrc_pktbuf_replace_snip(gnrc_pktsnip_t *pkt, gnrc_pktsnip_t *old, gnrc_pktsnip_t *add);

/**
 * @brief   Reverses snip order of a packet in a write-protected manner.
 *
 * This can be used to change the send/receive order of a packet (see
 * @ref gnrc_pktsnip_t)
 *
 * @note    @p pkt is released on failure.
 *
 * @param[in] pkt   A packet. When this function fails (due to a full packet
 *                  packet buffer) @p pkt will be released.
 *
 * @return  The reversed version of @p pkt on success
 * @return  NULL, when there is not enough space in the packet buffer to reverse
 *          the packet in a write-protected manner. @p pkt is released in that
 *          case.
 */
gnrc_pktsnip_t *gnrc_pktbuf_reverse_snips(gnrc_pktsnip_t *pkt);

/**
 * @brief Duplicates pktsnip chain upto (including) a snip with the given type
 *        as a continuous snip.
 *
 *          Example:
 *              Input:
 *                                                                  buffer
 *              +---------------------------+                      +------+
 *              | size = 8                  | data       +-------->|      |
 *              | type = NETTYPE_IPV6_EXT   |------------+         +------+
 *              +---------------------------+                      .      .
 *                    | next                                       .      .
 *                    v                                            .      .
 *              +---------------------------+                      +------+
 *              | size = 40                 | data    +----------->|      |
 *              | type = NETTYPE_IPV6       |---------+            +------+
 *              +---------------------------+                      .      .
 *                    | next                                       .      .
 *                    v
 *              +---------------------------+                      +------+
 *              | size = 14                 | data +-------------->|      |
 *              | type = NETTYPE_NETIF      |------+               +------+
 *              +---------------------------+                      .      .
 *
 *
 *              Output:
 *                                                                  buffer
 *              +---------------------------+                      +------+
 *              | size = 48                 | data       +-------->|      |
 *              | type = NETTYPE_IPV6       |------------+         |      |
 *              +---------------------------+                      |      |
 *                    |                                            +------+
 *                    |                                            .      .
 *                    | next                                       .      .
 *                    v
 *              +---------------------------+                      +------+
 *              | size = 14                 | data +-------------->|      |
 *              | type = NETTYPE_NETIF      |------+               +------+
 *              +---------------------------+                      .      .
 *
 *        The original snip is keeped as is except `users` decremented.
 *
 * @deprecated  This function breaks the abstraction of `gnrc_pktbuf` and its
 *              only user within the RIOT code base `gnrc_ipv6_ext` was reworked
 *              so it isn't needed anymore.
 *              It will be removed after the 2019.04 release.
 *
 * @param[in,out] pkt   The snip to duplicate.
 * @param[in]     type  The type of snip to stop duplication.
 *
 * @return The duplicated snip, if succeeded.
 * @return NULL, if no space is left in the packet buffer.
 */
gnrc_pktsnip_t *gnrc_pktbuf_duplicate_upto(gnrc_pktsnip_t *pkt, gnrc_nettype_t type);

/**
 * @brief   Merge pktsnip chain to single pktsnip.
 *
 * Specifically it calls @ref gnrc_pktbuf_realloc_data() on @p pkt, then copies
 * the data of all following packet snips into that reallocated space, and
 * removes the packet snip the data was copied from afterwards.
 *
 * ### Example
 * #### Input
 *
 *                                                         buffer
 *     +---------------------------+                      +------+
 *     | size = 8                  | data       +-------->|      |
 *     | type = NETTYPE_IPV6       |------------+         +------+
 *     +---------------------------+                      .      .
 *           | next                                       .      .
 *           v                                            .      .
 *     +---------------------------+                      +------+
 *     | size = 40                 | data    +----------->|      |
 *     | type = NETTYPE_UDP        |---------+            +------+
 *     +---------------------------+                      .      .
 *           | next                                       .      .
 *           v
 *     +---------------------------+                      +------+
 *     | size = 14                 | data +-------------->|      |
 *     | type = NETTYPE_UNDEF      |------+               +------+
 *     +---------------------------+                      .      .
 *
 *
 * #### Output
 *
 *                                                         buffer
 *     +---------------------------+                      +------+
 *     | size = 62                 | data       +-------->|      |
 *     | type = NETTYPE_IPV6       |------------+         |      |
 *     +---------------------------+                      |      |
 *                                                        |      |
 *                                                        |      |
 *                                                        |      |
 *                                                        +------+
 *                                                                 .      .
 *
 * @warning @p pkt needs to write protected before calling this function.
 * @note    Packets in receive order need to call
 *          @ref gnrc_pktbuf_reverse_snips() first to get the data in the
 *          correct order.
 *
 * @param[in,out] pkt   The snip to merge.
 *
 * @return  0, on success
 * @return  ENOMEM, if no space is left in the packet buffer.
 */
int gnrc_pktbuf_merge(gnrc_pktsnip_t *pkt);

#ifdef DEVELHELP
/**
 * @brief   Prints some statistics about the packet buffer to stdout.
 *
 * @note    Only available with DEVELHELP defined.
 *
 * @details Statistics include maximum number of reserved bytes.
 */
void gnrc_pktbuf_stats(void);
#endif

/* for testing */
#ifdef TEST_SUITES
/**
 * @brief   Checks if packet buffer is empty
 *
 * @return  true, if packet buffer is empty
 * @return  false, if packet buffer is not empty
 */
bool gnrc_pktbuf_is_empty(void);

/**
 * @brief   Checks if the implementation's internal invariants still uphold
 *
 * @return  true, the packet buffer is sane.
 * @return  false, the packet buffer is insane.
 */
bool gnrc_pktbuf_is_sane(void);
#endif

#ifdef __cplusplus
}
#endif

#endif /* NET_GNRC_PKTBUF_H */
/** @} */
