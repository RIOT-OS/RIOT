/*
 * Copyright (C) 2020 Locha Inc
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_rfc5444 RFC 5444
 * @ingroup     net
 * @brief
 * @see         <a href="https://tools.ietf.org/html/rfc5444">
 *                  RFC 5444 - Generalized Mobile Ad Hoc Network (MANET) Packet/Message Format
 *              </a>
 * @{
 *
 * @file
 * @brief
 *
 * @author      Jean Pierre Dudey <jeandudey@hotmail.com>
 */
#ifndef NET_RFC5444_HDR_H
#define NET_RFC5444_HDR_H

#include <stdint.h>
#include <stdlib.h>
#include <errno.h>

#include "byteorder.h"
#include "bitarithm.h"

#include "net/rfc5444/tlv.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Packet header flags
 * @{
 */
#define RFC5444_PKT_HDR_FLAG_HAS_SEQ_NUM (BIT3) /**< Has seq num? */
#define RFC5444_PKT_HDR_FLAG_HAS_TLV     (BIT2) /**< Has TLV block? */
/** @} */

/**
 * @brief   Packet header
 *
 * @see [RFC 5444, Section 5.1]
 *      (https://tools.ietf.org/html/rfc5444#section-5.1)
 */
typedef struct __attribute__((packed)) {
    uint8_t version_and_flags;     /**< Packet version and flags */
} rfc5444_pkt_hdr_t;

/**
 * @brief   Packet header version
 *
 * @pre @p hdr != NULL
 *
 * @param[in]   hdr Pointer to packet header.
 *
 * @return uint8_t Version, it MUST be 0 as specified by the standard.
 */
static inline uint8_t rfc5444_pkt_hdr_version(const rfc5444_pkt_hdr_t *hdr)
{
    return (hdr->version_and_flags & 0xF0) >> 4;
}

/**
 * @brief   Packet header flags.
 *
 * @pre @p hdr != NULL
 *
 * @param[in]   hdr Pointer to packet header.
 *
 * @return uint8_t Flags.
 */
static inline uint8_t rfc5444_pkt_hdr_flags(const rfc5444_pkt_hdr_t *hdr)
{
    return hdr->version_and_flags & 0x0F;
}

/**
 * @brief   Packet header sequence number.
 *
 * @pre @p hdr != NULL
 *
 * @param[in]   hdr Pointer to packet header.
 *
 * @return Sequence number or -1 if the packet doesn't provide a sequence
 *         number.
 */
static inline int rfc5444_pkt_hdr_seq_num(const rfc5444_pkt_hdr_t *hdr)
{
    if (rfc5444_pkt_hdr_flags(hdr) & RFC5444_PKT_HDR_FLAG_HAS_SEQ_NUM) {
        /* Read BE uint16_t at <pkt-seq-num>? location */
        const uint8_t *ptr = ((const uint8_t *)hdr) + sizeof(rfc5444_pkt_hdr_t);
        return (int)byteorder_bebuftohs(ptr);
    }

    return -1;
}

/**
 * @brief   Packet header TLV block.
 *
 * @pre @p != NULL
 *
 * @param[in]   hdr Pointer to packet header.
 *
 * @return Pointer to TLV block on success.
 * @return NULL if the packet doesnt't provide a TLV block.
 * @return NULL if @p hdr is NULL.
 */
static inline rfc5444_tlv_block_t *rfc5444_pkt_hdr_tlv_block(
    rfc5444_pkt_hdr_t *hdr)
{
    if (hdr == NULL) {
        return NULL;
    }

    const uint8_t flags = rfc5444_pkt_hdr_flags(hdr);

    if (flags & RFC5444_PKT_HDR_FLAG_HAS_TLV) {
        const size_t seqnum_size =
            (flags & RFC5444_PKT_HDR_FLAG_HAS_SEQ_NUM) ? sizeof(uint16_t) : 0;

        uint8_t *ptr =
            (((uint8_t *)hdr) + sizeof(rfc5444_pkt_hdr_t) + seqnum_size);
        return (rfc5444_tlv_block_t *)ptr;
    }

    return NULL;
}

/**
 * @brief   Total length of the packet header
 *
 * @pre @p hdr = NULL
 *
 * @param[in]   hdr  Pointer to the packet header.
 * @param[in]   size Size in bytes of the entire packet.
 *
 * @return The size of the packet header in bytes including the seqnum and TLV
 *         block if any, on success.
 * @return -EINVAL on invalid packet size.
 * @return -EINVAL if @p hdr is NULL.
 */
static inline int rfc5444_pkt_hdr_sizeof(rfc5444_pkt_hdr_t *hdr, int size)
{
    if (hdr == NULL) {
        return -EINVAL;
    }

    const uint8_t flags = rfc5444_pkt_hdr_flags(hdr);
    int count = sizeof(rfc5444_pkt_hdr_t);

    /* verify that the packet does have space for the packet header version
     * and flags */
    if (size <= count) {
        return -EINVAL;
    }

    /* calculate the space of the sequence number if any */
    if (flags & RFC5444_PKT_HDR_FLAG_HAS_SEQ_NUM) {
        count += sizeof(uint16_t);
        /* verify that the packet does have space for the sequence number */
        if (size <= count) {
            return -EINVAL;
        }
    }

    /* calculate the space of the TLV block if any */
    if (flags & RFC5444_PKT_HDR_FLAG_HAS_TLV) {
        /* verify that the packet does have space at least for the TLV block
         * length */
        count += sizeof(rfc5444_tlv_block_t);
        if (size <= count) {
            return -EINVAL;
        }

        uint8_t *ptr = (((uint8_t *)hdr) + count);

        count += (int)byteorder_ntohs(((rfc5444_tlv_block_t *)ptr)->length);

        /* verify that the packet does have space at least for the entire TLV
         * block */
        if (size <= count) {
            return -EINVAL;
        }
    }

    return count;
}

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* NET_RFC5444_HDR_H */
/** @} */
