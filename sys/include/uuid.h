/*
 * Copyright (C) 2018 Freie Universität Berlin
 * Copyright (C) 2018 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_uuid RFC 4122 compliant UUID's
 * @ingroup     sys
 * @brief       Provides RFC 4122 compliant UUID's
 *
 * This module provides RFC 4122 compliant UUID generation. The UUID stored in
 * @ref uuid_t struct is stored in network byte order.
 *
 * @{
 *
 * @file
 * @brief       [RFC 4122](https://tools.ietf.org/html/rfc4122) UUID functions
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 */

#ifndef UUID_H
#define UUID_H

#include <stdbool.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "byteorder.h"

#ifdef __cplusplus
extern "C" {
#endif

#define UUID_NODE_LEN    (6U)   /**< Size of the node identifier in bytes */

#define UUID_STR_LEN     (36U)  /**< Size of a string UUID without null character */

/**
 * @name UUID version identifiers
 * @{
 */
#define UUID_V1         (0x01)  /**< Type 1 UUID - timestamp based      */
#define UUID_V2         (0x02)  /**< Type 2 UUID - DCE Security version */
#define UUID_V3         (0x03)  /**< Type 3 UUID - Name based with MD5  */
#define UUID_V4         (0x04)  /**< Type 4 UUID - Random generated     */
#define UUID_V5         (0x05)  /**< Type 5 UUID - Name based with SHA1 */
/** @} */

/**
 * @name Version part of the time_hi field
 */
#define UUID_VERSION_MASK   (0xF000)

/**
 * @brief UUID layout
 *
 * Directly from [rfc4122](https://tools.ietf.org/html/rfc4122#section-4.1.2)
 */
typedef struct __attribute__((packed)) {
    network_uint32_t time_low;      /**< The low field of the timestamp       */
    network_uint16_t time_mid;      /**< The middle field of the timestamp    */
    network_uint16_t time_hi;       /**< The high field of the timestamp
                                     *  multiplexed with the version  number */
    uint8_t clk_seq_hi_res;         /**< The high field of the clock sequence
                                     *  Multiplexed with the variant         */
    uint8_t clk_seq_low;            /**< The low field of the clock sequence  */
    uint8_t node[UUID_NODE_LEN];    /**< The spatially unique node identifier */
} uuid_t;


/**
 * @name Namespace IDs from RFC4122
 *
 * Copied from [rfc4122 Appendix
 * C](https://tools.ietf.org/html/rfc4122#appendix-C)
 *
 * @{
 */
extern const uuid_t uuid_namespace_dns;     /**< DNS namespace UUID */
extern const uuid_t uuid_namespace_url;     /**< URL namespace UUID */
extern const uuid_t uuid_namespace_iso;     /**< ISO OID namespace UUID */
extern const uuid_t uuid_namespace_x500;    /**< X.500 DN namespace UUID */
/** @} */

/**
 * Generate a version 3(md5 based) UUID from a namespace and a byte array
 *
 * @param[out]  uuid    UUID struct to fill
 * @param[in]   ns      Namespace UUID
 * @param[in]   name    Ptr to byte array to use as name part
 * @param[in]   len     Length of the byte array
 */
void uuid_v3(uuid_t *uuid, const uuid_t *ns, const uint8_t *name, size_t len);


/**
 * Generate a version 4(Full random) UUID
 *
 * @param[out]  uuid    UUID struct to fill
 */
void uuid_v4(uuid_t *uuid);

/**
 * Generate a version 5(sha1 based) UUID from a namespace and a byte array
 *
 * @param[out]  uuid    UUID struct to fill
 * @param[in]   ns      Namespace UUID
 * @param[in]   name    Ptr to byte array to use as name part
 * @param[in]   len     Length of the byte array
 */
void uuid_v5(uuid_t *uuid, const uuid_t *ns, const uint8_t *name, size_t len);

/**
 * Retrieve the type number of a UUID
 *
 * @param[in]   uuid    UUID to retrieve version number from
 *
 * @return              Version number
 */
static inline unsigned uuid_version(const uuid_t *uuid)
{
    uint16_t time_hi_vers = byteorder_ntohs(uuid->time_hi);

    return (time_hi_vers & 0xF000) >> 12;
}

/**
 * Compare two UUID's
 *
 * @param[in]   uuid1   First uuid to compare
 * @param[in]   uuid2   Second uuid to compare
 *
 * @return              True when equal
 */
static inline bool uuid_equal(const uuid_t *uuid1, const uuid_t *uuid2)
{
    return (memcmp(uuid1, uuid2, sizeof(uuid_t)) == 0);
}

/**
 * @brief   Generate an UUID string from an UUID structure
 *
 * @param[in]   uuid    UUID
 * @param[out]  str     null-terminated UUID string, must be at least UUID_STR_LEN + 1 bytes
 */
void uuid_to_string(const uuid_t *uuid, char *str);

/**
 * @brief   Populate an UUID structure from an UUID string
 *
 * @param[out]  uuid    out UUID
 * @param[in]   str     null-terminated input UUID string, must be UUID_STR_LEN bytes
 *
 * @return 0 on success, < 0 if @p str is not valid
 */
int uuid_from_string(uuid_t *uuid, const char *str);

#ifdef __cplusplus
}
#endif
#endif /* UUID_H */
/** @} */
