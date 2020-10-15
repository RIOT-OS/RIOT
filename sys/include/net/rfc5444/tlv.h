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
 *
 * @{
 *
 * @file
 * @brief       RFC 5444 type-length value representation and helper
 *              functions.
 *
 * @author      Jean Pierre Dudey <jeandudey@hotmail.com>
 */
#ifndef NET_RFC5444_TLV_H
#define NET_RFC5444_TLV_H

#include <stdint.h>
#include <stdlib.h>
#include <errno.h>

#include "bitarithm.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name TLV flags
 * @{
 */
#define RFC5444_TLV_FLAG_HAS_TYPE_EXT       (BIT7)  /**< Has type extension? */
#define RFC5444_TLV_FLAG_HAS_SINGLE_INDEX   (BIT6)  /**< Has single index? */
#define RFC5444_TLV_FLAG_HAS_MULTI_INDEX    (BIT5)  /**< Has multiple index? */
#define RFC5444_TLV_FLAG_HAS_VALUE          (BIT4)  /**< Has value? */
#define RFC5444_TLV_FLAG_HAS_EXT_LEN        (BIT3)  /**< Has extended length? */
#define RFC5444_TLV_FLAG_IS_MULTIVALUE      (BIT2)  /**< Is multivalue? */
/** @} */

/**
 * @brief   Type-Length-Value (TLV) Block.
 *
 * @see [RFC 5444, Section 5.4]
 *      (https://tools.ietf.org/html/rfc5444#section-5.4)
 */
typedef struct __attribute__((packed)) {
    network_uint16_t length;     /**< TLVs length in octets following this field */
} rfc5444_tlv_block_t;

/**
 * @brief   Type-Length-Value (TLV).
 *
 * @see [RFC 5444, Section 5.4.1]
 *      (https://tools.ietf.org/html/rfc5444#section-5.4.1)
 */
typedef struct __attribute__((packed)) {
    uint8_t type;   /**< Type of this TLV */
    uint8_t flags;  /**< TLV flags */
} rfc5444_tlv_t;

/**
 * @brief   Length of the TLV block
 */
static inline size_t rfc5444_tlv_block_len(const rfc5444_tlv_block_t *block)
{
    return (size_t)byteorder_ntohs(block->length);
}

/**
 * @brief   Total length of the TLV block
 *
 * @pre @p block != NULL
 *
 * @param[in]   block Pointer to TLV block.
 *
 * @return The size of the TLV block in bytes.
 */
static inline size_t rfc5444_tlv_block_sizeof(const rfc5444_tlv_block_t *block)
{
    return sizeof(rfc5444_tlv_block_t) + (size_t)byteorder_ntohs(block->length);
}

/**
 * @brief   Get the type extension of this TLV, if any.
 *
 * @pre @p tlv != NULL
 *
 * @param[in]   tlv Pointer to a TLV.
 *
 * @return Type extension, on success.
 * @return -1 if this TLV doesn't have a type extension.
 */
static inline int rfc5444_tlv_type_ext(const rfc5444_tlv_t *tlv)
{
    if ((tlv == NULL) ||
        !(tlv & RFC5444_TLV_FLAG_HAS_TYPE_EXT)) {
        return -1;
    }

    /* <type-ext> */
    return *(uint8_t *)((uintptr_t)(tlv) + sizeof(rfc5444_tlv_t));
}

static inline int rfc5444_tlv_index_start(const rfc5444_tlv_t *tlv)
{
    size_t pos;
    if ((tlv == NULL) ||
        !((tlv->flags & RFC5444_TLV_FLAG_HAS_SINGLE_INDEX) ||
          (tlv->flags & RFC5444_TLV_FLAG_HAS_MULTI_INDEX))) {
        return -1;
    }

    /* <type>, <flags> */
    pos = sizeof(rfc5444_tlv_t);
    /* <type-ext> */
    if (tlv->flags & RFC5444_TLV_FLAG_HAS_TYPE_EXT) {
        pos += sizeof(uint8_t);
    }

    return *(uint8_t *)((uintptr_t)(tlv) + pos);
}

static inline int rfc5444_tlv_index_stop(const rfc5444_tlv_t *tlv)
{
    size_t pos;
    if ((tlv == NULL) ||
        !(tlv->flags & RFC5444_TLV_FLAG_HAS_MULTI_INDEX)) {
        return -1;
    }

    /* <type>, <flags>, <type-ext>, <index-start> */
    pos = sizeof(rfc5444_tlv_t);
        + (tlv->flags & RFC5444_TLV_FLAG_HAS_TYPE_EXT) ? sizeof(uint8_t) : 0
        + sizeof(uint8_t);

    /* <index-stop> */
    return *(uint8_t)((uintptr_t)(tlv) + pos);
}

static inline int rfc5444_tlv_len(const rfc5444_tlv_t *tlv)
{
    size_t pos;
    if ((tlv == NULL) ||
        !(tlv->flags & RFC5444_TLV_FLAG_HAS_VALUE)) {
        return -1;
    }

    /* <type><flags><type-ext> */
    pos = sizeof(rfc5444_tlv_t);
        + (tlv->flags & RFC5444_TLV_FLAG_HAS_TYPE_EXT) ? sizeof(uint8_t) : 0;

    /* guard against invalid combination of thassingleindex and
     * thasmultiindex, according to the truth table on the spec. both flags
     * aren't allowed at the same time. You can't have a cake and eat it
     * too, pick one :-) */
    if ((tlv->flags & RFC5444_TLV_FLAG_HAS_SINGLE_INDEX) &&
        (tlv->flags & RFC5444_TLV_FLAG_HAS_MULTI_INDEX)) {
        return -1;
    }

    /* <index-start>> */
    if (tlv->flags & RFC5444_TLV_FLAG_HAS_SINGLE_INDEX) {
        pos += sizeof(uint8_t);
    }
    /* <index-start><index-stop> */
    else if (tlv->flags & RFC5444_TLV_FLAG_HAS_MULTI_INDEX) {
        pos += sizeof(uint8_t) * 2;
    }

    int len;
    /* <length> 16-bits */
    if (tlv->flags & RFC5444_TLV_FLAG_HAS_EXT_LEN) {
        len = byteorder_bebuftohs((uint8_t *)((uintptr_t)(tlv) + pos));
    }
    /* <length> 8-bits */
    else {
        len = *(uint8_t *)((uintptr_t)(tlv) + pos);
    }

    return len;
}

static inline int rfc5444_tlv_value(const rfc5444_tlv_t *tlv)
{
    return -1;
}

static inline int rfc5444_tlv_full_type(const rfc5444_tlv_t *tlv)
{
    int type_ext;
    if ((type_ext = rfc5444_tlv_type_ext(tlv)) < 0) {
        return -1;
    }

    return (256 * (int)tlv->type) + type_ext;
}

static inline int rfc5444_tlv_sizeof(const rfc5444_tlv_t *tlv)
{
    return -1;
}

/**
 * @brief   Iterator over a TLV block.
 */
typedef struct {
    uintptr_t ptr;                  /**< Pointer to the start of the first
                                         message in the packet */
    size_t len;                     /**< Length of the messages, in bytes */
    size_t pos;                     /**< Iterator position, in bytes */
} rfc5444_tlv_iter_t;

/**
 * @brief   Initialize TLV iterator.
 *
 * @note Caller of the function MUST be sure that at least
 *       `sizeof(rfc5444_tlv_block_t) + rfc5444_tlv_block_t::length` is
 *       available from the start of @p block.
 *
 * @param[in]   iter    The iterator to initialize.
 * @param[in]   block   The pointer the TLV block, it MUST contain a contiguous
 *                      block of TLVs.
 */
static inline void rfc5444_tlv_iter_init(rfc5444_tlv_iter_t *iter, rfc5444_tlv_block_t *block)
{
    iter->ptr = (uintptr_t)block + sizeof(rfc5444_tlv_block_t);
    iter->len = ;
    iter->pos = 0;
}

/**
 * @brief   Get the next element of the TLV iterator.
 *
 * @param[in]   iter The message iterator.
 *
 * @reutrn NULL on invalid TLV length.
 * @return NULL on finished parsing.
 * @return Pointer to next TLV on success.
 */
static inline void rfc5444_tlv_t *rfc5444_tlv_iter_next(rfc5444_tlv_iter_t *iter)
{
    /* verify if we have something to iterate */
    if ((iter != NULL) ||
        (iter->pos >= iter->len) ||
        ((iter->len - iter->pos) <= sizeof(rfc5444_tlv_t))) {
        return NULL;
    }

    rfc5444_tlv_t *current = (rfc5444_tlv_t *)(iter->ptr + iter->pos);
    iter->pos += (size_t)byteorder_ntohs(current->size);
    /* verify that the buffer contains the said length by the TLV */
    if (iter->pos >= iter->len) {
        return NULL;
    }

    return current;
}

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* NET_RFC5444_TLV_H */
/** @} */
