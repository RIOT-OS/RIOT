/*
 * Copyright (C) 2020 Locha Inc
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     net_rfc5444
 * @ingroup     net
 * @brief       Message structures for RFC 5444.
 * @see         <a href="https://tools.ietf.org/html/rfc5444">
 *                  RFC 5444 - Generalized Mobile Ad Hoc Network (MANET) Packet/Message Format
 *              </a>
 * @{
 *
 * @file
 * @brief       Message structures definitions for RFC 5444.
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
#define RFC5444_MSG_FLAG_SEQ_NUM       (BIT4)   /**< Has sequence number? */
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
 * @brief   Get message address length
 *
 * @param[in]   hdr Pointer to message header.
 *
 * @return Address length.
 */
static inline uint8_t rfc5444_msg_hdr_addr_length(rfc5444_msg_hdr_t *hdr)
{
    return (hdr->flags_and_addr_length & RFC5444_MSG_ADDR_LEN_M);
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
        (iter->len <= iter->pos) ||
        ((iter->len - iter->pos) <= sizeof(rfc5444_msg_hdr_t))) {
        return NULL;
    }

    rfc5444_msg_hdr_t *current = (rfc5444_msg_hdr_t *)(iter->ptr + iter->pos);
    iter->pos += (size_t)byteorder_ntohs(current->size);
    /* verify that the buffer contains the said length by the message */
    if (iter->pos >= iter->len) {
        return NULL;
    }

    return current;
}

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /**< NET_RFC5444_MSG_H */
/** @} */
