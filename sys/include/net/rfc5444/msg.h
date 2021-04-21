/*
 * Copyright (C) 2020 Locha Inc
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_rfc5444_msg RFC 5444 messages
 * @ingroup     net_rfc5444
 *
 * @brief       RFC 5444 message structures and definitions.
 *
 * @see <a href="https://tools.ietf.org/html/rfc5444#section-5.2">
 *          RFC 5444, Section 5.2
 *      </a>
 * @{
 *
 * @file
 * @brief       RFC 5444 message structures and definitions.
 *
 * @author      Jean Pierre Dudey <jeandudey@hotmail.com>
 */

#ifndef NET_RFC5444_MSG_H
#define NET_RFC5444_MSG_H

#include <stdint.h>

#include "bitarithm.h"
#include "byteorder.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Message flags
 * @{
 */
#define RFC5444_MSG_FLAG_HAS_ORIG      (BIT7)   /**< Has originator address? */
#define RFC5444_MSG_FLAG_HAS_HOP_LIMIT (BIT6)   /**< Has hop limit? */
#define RFC5444_MSG_FLAG_HAS_HOP_COUNT (BIT5)   /**< Has hop count? */
#define RFC5444_MSG_FLAG_HAS_SEQ_NUM   (BIT4)   /**< Has sequence number? */
/** @} */

#define RFC5444_MSG_ADDR_LEN_M         (0x0F)   /**< Address length mask */

/**
 * @brief   Message header
 *
 * @see [RFC 5444, Section 5.2]
 *      (https://tools.ietf.org/html/rfc5444#section-5.2)
 */
typedef struct __attribute__((packed)) {
    uint8_t type;                   /**< Message type */
    uint8_t flags_and_addr_length;  /**< Flags and address length fields */
    network_uint16_t size;          /**< Total size of the message including
                                         this header, in bytes */
} rfc5444_msg_hdr_t;

/**
 * @brief   Get message address length.
 *
 * The maximum address length is 16-bytes.
 *
 * @pre @p hdr != NULL
 *
 * @param[in]   hdr Pointer to message header.
 *
 * @return Address length, in bytes.
 * @return -1 if hdr is NULL.
 */
static inline int rfc5444_msg_hdr_addr_len(const rfc5444_msg_hdr_t *hdr)
{
    if (hdr == NULL) {
        return -1;
    }

    return (hdr->flags_and_addr_length & RFC5444_MSG_ADDR_LEN_M);
}

/**
 * @brief   Get the originator address of a message.
 *
 * @pre @p hdr != NULL
 *
 * @param[in]   hdr Pointer to message header.
 *
 * @return Pointer to originator address, with a size of
 *         @ref rfc5444_msg_hdr_addr_len.
 * @return NULL if the message doesn't have an originator address.
 */
static inline uint8_t *rfc5444_msg_hdr_orig_addr(rfc5444_msg_hdr_t *hdr)
{
    if ((hdr == NULL) ||
        !(hdr->flags_and_addr_length & RFC5444_MSG_FLAG_HAS_ORIG)) {
        return NULL;
    }

    /* <orig-addr> */
    return (uint8_t *)((uintptr_t)hdr + sizeof(rfc5444_msg_hdr_t));
}

/**
 * @brief   Get the hop limit of a message.
 *
 * @pre @p hdr != NULL
 *
 * @param[in]   hdr Pointer to message header.
 *
 * @return Hop limit, positive integer <= UINT8_MAX.
 * @return -1 if the message doesn't contain a hop limit.
 */
static inline int rfc5444_msg_hdr_hop_limit(const rfc5444_msg_hdr_t *hdr)
{
    size_t pos;
    if ((hdr == NULL) ||
        !(hdr->flags_and_addr_length & RFC5444_MSG_FLAG_HAS_HOP_LIMIT)) {
        return -1;
    }

    /* <msg-type><msg-flags><msg-addr-length><msg-size><msg-orig-addr>? */
    bool has_orig = hdr->flags_and_addr_length & RFC5444_MSG_FLAG_HAS_ORIG;
    pos = sizeof(rfc5444_msg_hdr_t)
        + (has_orig ? (size_t)rfc5444_msg_hdr_addr_len(hdr) : 0);

    return *(uint8_t *)((uintptr_t)hdr + pos);
}

/**
 * @brief   Get the hop count of a message.
 *
 * @pre @p hdr != NULL
 *
 * @param[in]   hdr Pointer to message header.
 *
 * @return Hop count, positive integer <= UINT8_MAX.
 * @return -1 if the message doesn't contain a hop limit.
 */
static inline int rfc5444_msg_hdr_hop_count(const rfc5444_msg_hdr_t *hdr)
{
    size_t pos;
    if ((hdr == NULL) ||
        !(hdr->flags_and_addr_length & RFC5444_MSG_FLAG_HAS_HOP_COUNT)) {
        return -1;
    }

    /* <msg-type><msg-flags><msg-addr-length><msg-size><msg-orig-addr>?
     * <msg-hop-limit> */
    bool has_orig = hdr->flags_and_addr_length & RFC5444_MSG_FLAG_HAS_ORIG;
    bool has_hoplimit = hdr->flags_and_addr_length & RFC5444_MSG_FLAG_HAS_HOP_LIMIT;
    pos = sizeof(rfc5444_msg_hdr_t)
        + (has_orig ? (size_t)rfc5444_msg_hdr_addr_len(hdr) : 0)
        + (has_hoplimit ? sizeof(uint8_t) : 0);

    /* <msg-hop-count> */
    return *(uint8_t *)((uintptr_t)hdr + pos);
}

/**
 * @brief   Get the sequence number of a message.
 *
 * @pre @p hdr != NULL
 *
 * @param[in]   hdr Pointer to message header.
 *
 * @return Sequence number, positive integer <= UINT16_MAX.
 * @return -1 if the message doesn't contain a sequence number.
 */
static inline int rfc5444_msg_hdr_seq_num(const rfc5444_msg_hdr_t *hdr)
{
    size_t pos;
    if ((hdr == NULL) ||
        !(hdr->flags_and_addr_length & RFC5444_MSG_FLAG_HAS_SEQ_NUM)) {
        return -1;
    }

    /* <msg-type><msg-flags><msg-addr-length><msg-size><msg-orig-addr>?
     * <msg-hop-limit><msg-hop-count> */
    bool has_orig = hdr->flags_and_addr_length & RFC5444_MSG_FLAG_HAS_ORIG;
    bool has_hoplimit = hdr->flags_and_addr_length & RFC5444_MSG_FLAG_HAS_HOP_LIMIT;
    bool has_hopcount = hdr->flags_and_addr_length & RFC5444_MSG_FLAG_HAS_HOP_COUNT;
    pos = sizeof(rfc5444_msg_hdr_t)
        + (has_orig ? (size_t)rfc5444_msg_hdr_addr_len(hdr) : 0)
        + (has_hoplimit ? sizeof(uint8_t) : 0)
        + (has_hopcount ? sizeof(uint8_t) : 0);

    /* <msg-seq-num> */
    return byteorder_bebuftohs((uint8_t *)((uintptr_t)hdr + pos));
}
/**
 * @brief   Total length of the message header in bytes.
 *
 * @pre @p hdr != NULL
 *
 * @param[in]   hdr Pointer to message header.
 *
 * @return Length of the header, in bytes.
 */
static inline int rfc5444_msg_hdr_sizeof(const rfc5444_msg_hdr_t *hdr)
{
    bool has_orig = hdr->flags_and_addr_length & RFC5444_MSG_FLAG_HAS_ORIG;
    bool has_hoplimit = hdr->flags_and_addr_length & RFC5444_MSG_FLAG_HAS_HOP_LIMIT;
    bool has_hopcount = hdr->flags_and_addr_length & RFC5444_MSG_FLAG_HAS_HOP_COUNT;
    bool has_seqnum = hdr->flags_and_addr_length & RFC5444_MSG_FLAG_HAS_SEQ_NUM;

    return sizeof(rfc5444_msg_hdr_t)
         + (has_orig ? (size_t)rfc5444_msg_hdr_addr_len(hdr) : 0)
         + (has_hoplimit ? sizeof(uint8_t) : 0)
         + (has_hopcount ? sizeof(uint8_t) : 0)
         + (has_seqnum ? sizeof(uint16_t): 0);
}

/**
 * @brief   Iterator over the messages
 */
typedef struct {
    uint8_t *ptr;                   /**< Pointer to the start of the first
                                         message in the packet */
    size_t len;                     /**< Length of the messages, in bytes */
    size_t pos;                     /**< Iterator position, in bytes */
} rfc5444_msg_iter_t;

/**
 * @brief   Initialize message iterator.
 *
 * The @p len parameter should be calculated before calling the function,
 * it should be something along the lines of:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.c}
 * int packet_len = recv(...);
 * int len = packet_len - rfc5444_pkt_hdr_sizeof(hdr);
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * The @p ptr parameter is a pointer to the start of the messages,
 * just after the packet header.
 *
 * @param[in]   iter The iterator to initialize.
 * @param[in]   ptr  The pointer to the start of the first message,
 *                   it MUST be a contiguous block of messages.
 * @param[in]   len  The length of the message block, this determines
 *                   the parsing behaviour, as this determines when it's
 *                   finished.
 */
static inline void rfc5444_msg_iter_init(rfc5444_msg_iter_t *iter, void *ptr,
                                         size_t len)
{
    iter->ptr = ptr;
    iter->len = len;
    iter->pos = 0;
}

/**
 * @brief   Get next element of the message iterator.
 *
 * @param[in]   iter The message iterator.
 *
 * @return NULL on invalid message length
 * @return NULL on finished parsing
 * @return Pointer to next message header on success
 */
static inline rfc5444_msg_hdr_t *rfc5444_msg_iter_next(rfc5444_msg_iter_t *iter)
{
    /* verify if we have something to iterate */
    if ((iter == NULL) ||
        (iter->pos >= iter->len) ||
        ((iter->len - iter->pos) <= sizeof(rfc5444_msg_hdr_t))) {
        return NULL;
    }

    rfc5444_msg_hdr_t *current = (rfc5444_msg_hdr_t *)(iter->ptr + iter->pos);
    size_t msg_len = byteorder_ntohs(current->size);
    if ((iter->pos - iter->len) < msg_len) {
        return NULL;
    }
    iter->pos += msg_len;

    return current;
}

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /**< NET_RFC5444_MSG_H */
/** @} */
