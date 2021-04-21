/*
 * Copyright (C) 2020 Locha Inc
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_rfc5444_tlv RFC 5444 TLVs
 * @ingroup     net_rfc5444
 * @brief       RFC 5444 type-length value representation and helper
 *              functions.
 *
 * @see <a href="https://tools.ietf.org/html/rfc5444#section-5.4">
 *          RFC 5444, Section 5.4
 *      </a>
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
#include <stdbool.h>
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
        !(tlv->flags & RFC5444_TLV_FLAG_HAS_TYPE_EXT)) {
        return -1;
    }

    /* <type-ext> */
    return *(uint8_t *)((uintptr_t)(tlv) + sizeof(rfc5444_tlv_t));
}

/**
 * @brief   Get the index-start value of this TLV, if any.
 *
 * @pre @p tlv != NULL
 *
 * @param[in]   tlv Pointer to a TLV.
 *
 * @return index-start value.
 * @return -1 if @p tlv is NULL.
 * @return -1 if index-start is not provided.
 */
static inline int rfc5444_tlv_index_start(const rfc5444_tlv_t *tlv)
{
    size_t pos;
    if ((tlv == NULL) ||
        !((tlv->flags & RFC5444_TLV_FLAG_HAS_SINGLE_INDEX) ||
          (tlv->flags & RFC5444_TLV_FLAG_HAS_MULTI_INDEX))) {
        return -1;
    }

    /* <type><flags><type-ext> */
    pos = sizeof(rfc5444_tlv_t)
        + (tlv->flags & RFC5444_TLV_FLAG_HAS_TYPE_EXT ? sizeof(uint8_t) : 0);

    /* <index-start> */
    return *(uint8_t *)((uintptr_t)(tlv) + pos);
}

/**
 * @brief   Get the index-stop value of a TLV.
 *
 * @param[in]   tlv The TLV.
 *
 * @return index-stop value, positive integer.
 * @return -1 If the TLV doesn't have at least an <index-start> value.
 */
static inline int rfc5444_tlv_index_stop(const rfc5444_tlv_t *tlv)
{
    size_t pos;
    if ((tlv == NULL) ||
        !((tlv->flags & RFC5444_TLV_FLAG_HAS_SINGLE_INDEX) ||
          (tlv->flags & RFC5444_TLV_FLAG_HAS_MULTI_INDEX))) {
        return -1;
    }
    /* if it's only single index, then index-stop is equal to index-start */
    else if (!(tlv->flags & RFC5444_TLV_FLAG_HAS_MULTI_INDEX)) {
        return rfc5444_tlv_index_start(tlv);
    }

    /* <type><flags><type-ext><index-start> */
    pos = sizeof(rfc5444_tlv_t)
        + (tlv->flags & RFC5444_TLV_FLAG_HAS_TYPE_EXT) ? sizeof(uint8_t) : 0
        + sizeof(uint8_t);

    /* <index-stop> */
    return *(uint8_t *)((uintptr_t)(tlv) + pos);
}

/**
 * @brief   Get the length of a TLV.
 *
 * @param[in]   tlv The TLV.
 *
 * @return Length of the TLV in bytes, can be 0 if specified like that
 *         on the TLV.
 * @return -1 if TLV doesn't have a value, nor a length.
 */
static inline int rfc5444_tlv_len(const rfc5444_tlv_t *tlv)
{
    size_t pos;
    if ((tlv == NULL) ||
        !(tlv->flags & RFC5444_TLV_FLAG_HAS_VALUE)) {
        return -1;
    }

    bool is_single_idx = (tlv->flags & RFC5444_TLV_FLAG_HAS_SINGLE_INDEX) &&
                         !(tlv->flags & RFC5444_TLV_FLAG_HAS_MULTI_INDEX);
    bool is_multi_idx = tlv->flags & RFC5444_TLV_FLAG_HAS_MULTI_INDEX;

    /* <type><flags><type-ext>(<index-start><index-stop>)? */
    pos = sizeof(rfc5444_tlv_t)
        + (tlv->flags & RFC5444_TLV_FLAG_HAS_TYPE_EXT ? sizeof(uint8_t) : 0)
        + (is_single_idx ? sizeof(uint8_t) : (is_multi_idx ? sizeof(uint8_t) * 2
                                                           : 0));

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

/**
 * @brief   Get the value of a TLV.
 *
 * @param[in]   tlv The TLV.
 *
 * @return Pointer to the bytes of the value, with a length of
 *         @ref rfc5444_tlv_len.
 * @return NULL if the TLV doesn't provide a value.
 * @return NULL if the TLV length is 0.
 */
static inline uint8_t *rfc5444_tlv_value(rfc5444_tlv_t *tlv)
{
    int pos;

    int len;
    if ((len = rfc5444_tlv_len(tlv)) < 0) {
        return NULL;
    }

    /* provides a value but with a length of 0 */
    if (len == 0) {
        return NULL;
    }

    bool is_single_idx = (tlv->flags & RFC5444_TLV_FLAG_HAS_SINGLE_INDEX) &&
                         !(tlv->flags & RFC5444_TLV_FLAG_HAS_MULTI_INDEX);
    bool is_multi_idx = tlv->flags & RFC5444_TLV_FLAG_HAS_MULTI_INDEX;

    /* <type><flags><type-ext>(<index-start><index-stop>)?<length> */
    pos = sizeof(rfc5444_tlv_t)
        + (tlv->flags & RFC5444_TLV_FLAG_HAS_TYPE_EXT ? sizeof(uint8_t) : 0)
        + (is_single_idx ? sizeof(uint8_t) : (is_multi_idx ? sizeof(uint8_t) * 2
                                                           : 0))
        + (tlv->flags & RFC5444_TLV_FLAG_HAS_EXT_LEN ? sizeof(uint16_t)
                                                     : sizeof(uint8_t));

    /* <value> */
    return (uint8_t *)((uintptr_t)(tlv) + pos);
}

/**
 * @brief   Calculate the number of values in the <value> field
 *          of the TLV.
 *
 * If the flag #RFC5444_TLV_FLAG_IS_MULTIVALUE is present this function
 * can be used to calculate the number of values in the <value> field.
 *
 * This way each single value is equal to:
 *
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ {.c}
 * int single_value_len = rfc5444_tlv_len(tlv) /
 *                        rfc5444_tlv_number_values(tlv);
 * ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
 *
 * @param[in]   tlv The TLV.
 *
 * @return Number of values in a single TLV.
 * @return -1 if the TLV doesn't provide at least the <index-start> value.
 */
static inline int rfc5444_tlv_number_values(const rfc5444_tlv_t *tlv)
{
    int index_start;
    int index_stop;
    if ((index_start = rfc5444_tlv_index_start(tlv)) < 0 ||
        (index_stop = rfc5444_tlv_index_stop(tlv)) < 0) {
        return -1;
    }

    return index_stop - index_start + 1;
}

/**
 * @brief   Calculate the full type of a TLV.
 *
 * A full type is defined to be:
 *
 * `256 * <tlv-type> + tlv-type-ext`
 *
 * Where tlv-type-ext is equal to 0 if it isn't provided by the
 * TLV.
 *
 * @param[in]   tlv The TLV.
 *
 * @return Type extension, positive integer.
 */
static inline int rfc5444_tlv_full_type(const rfc5444_tlv_t *tlv)
{
    /* tlv-type-ext is 0 if not defined */
    int type_ext;
    if ((type_ext = rfc5444_tlv_type_ext(tlv)) < 0) {
        type_ext = 0;
    }

    return (256 * (int)tlv->type) + type_ext;
}

/**
 * @brief   Total length of a single TLV, in bytes.
 *
 * @param[in]   tlv The TLV.
 *
 * @return TLV size in bytes.
 */
static inline size_t rfc5444_tlv_sizeof(const rfc5444_tlv_t *tlv)
{
    size_t size;

    bool is_single_idx = (tlv->flags & RFC5444_TLV_FLAG_HAS_SINGLE_INDEX) &&
                         !(tlv->flags & RFC5444_TLV_FLAG_HAS_MULTI_INDEX);
    bool is_multi_idx = tlv->flags & RFC5444_TLV_FLAG_HAS_MULTI_INDEX;

    /* <type><flags><type-ext>(<index-start><index-stop>)? */
    size = sizeof(rfc5444_tlv_t)
         + (tlv->flags & RFC5444_TLV_FLAG_HAS_TYPE_EXT ? sizeof(uint8_t) : 0)
         + (is_single_idx ? sizeof(uint8_t) : (is_multi_idx ? sizeof(uint8_t) * 2
                                                            : 0));

    /* <length><value> */
    if (tlv->flags & RFC5444_TLV_FLAG_HAS_VALUE) {
        size += (tlv->flags & RFC5444_TLV_FLAG_HAS_EXT_LEN ? sizeof(uint16_t)
                                                           : sizeof(uint8_t));

        int tlv_len;
        if ((tlv_len = rfc5444_tlv_len(tlv)) > 0) {
            size += (size_t)tlv_len;
        }
    }

    return size;
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
    iter->len = rfc5444_tlv_block_len(block);
    iter->pos = 0;
}

/**
 * @brief   Get the next element of the TLV iterator.
 *
 * @param[in]   iter The message iterator.
 *
 * @return NULL on invalid TLV length.
 * @return NULL on finished parsing.
 * @return Pointer to next TLV on success.
 */
static inline rfc5444_tlv_t *rfc5444_tlv_iter_next(rfc5444_tlv_iter_t *iter)
{
    /* verify if we have something to iterate */
    if ((iter == NULL) ||
        (iter->pos >= iter->len) ||
        ((iter->len - iter->pos) <= sizeof(rfc5444_tlv_t))) {
        return NULL;
    }

    rfc5444_tlv_t *current = (rfc5444_tlv_t *)(iter->ptr + iter->pos);
    iter->pos += rfc5444_tlv_sizeof(current);

    return current;
}

#ifdef __cplusplus
} /* extern "C" */
#endif

#endif /* NET_RFC5444_TLV_H */
/** @} */
