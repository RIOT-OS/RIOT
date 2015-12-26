/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_sixlowpan_nd    6LoWPAN Neighbor Discovery
 * @ingroup     net_sixlowpan
 * @brief       Message types and helper functions for Neighbor Discovery Optimization for 6LoWPAN
 *              (6LoWPAN-ND)
 * @see <a href="https://tools.ietf.org/html/rfc6775#section-4">
 *          RFC 6775, section 4
 *      </a>
 * @{
 *
 * @file
 * @brief   Message type and helper function definitions for 6LoWPAN-ND
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef SIXLOWPAN_ND_H_
#define SIXLOWPAN_ND_H_

#include <stdint.h>

#include "byteorder.h"
#include "net/eui64.h"
#include "net/ipv6/addr.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @{
 * @name    Lengths for fixed length options
 * @brief   Options don't use bytes as their length unit, but 8 bytes.
 */
#define SIXLOWPAN_ND_OPT_AR_LEN                 (2U)
#define SIXLOWPAN_ND_OPT_ABR_LEN                (3U)
/**
 * @}
 */

/**
 * @{
 * @name    Status values for address registration option and DARs
 *
 * @see <a href="https://tools.ietf.org/html/rfc6775#section-4.1">
 *          RFC 6775, section 4.1
 *      </a>
 */
#define SIXLOWPAN_ND_STATUS_SUCCESS             (0) /**< success */
#define SIXLOWPAN_ND_STATUS_DUP                 (1) /**< duplicate address */
#define SIXLOWPAN_ND_STATUS_NC_FULL             (2) /**< neighbor cache full */
/**
 * @}
 */

/**
 * @{
 * @name    Flags for 6LoWPAN context option
 */
#define SIXLOWPAN_ND_OPT_6CTX_FLAGS_MASK        (0x1f)
#define SIXLOWPAN_ND_OPT_6CTX_FLAGS_C           (0x10)  /**< valid for compression */
#define SIXLOWPAN_ND_OPT_6CTX_FLAGS_CID_MASK    (0x0f)  /**< mask for CID */
/**
 * @}
 */

/**
 * @brief   Duplicate address request and confirmation message format.
 * @extends icmpv6_hdr_t
 *
 * @see <a href="https://tools.ietf.org/html/rfc6775#section-4.4">
 *          RFC 6775, section 4.4
 *      </a>
 */
typedef struct __attribute__((packed)) {
    uint8_t type;           /**< message type */
    uint8_t code;           /**< message code */
    network_uint16_t csum;  /**< checksum */
    uint8_t status;         /**< indicates registration status in DAC; must be 0 in DAR. */
    uint8_t resv;           /**< reserved field */
    network_uint16_t ltime; /**< registration lifetime */
    eui64_t eui64;          /**< EUI-64 */
    ipv6_addr_t addr;       /**< registered address */
} sixlowpan_nd_da_t;

/**
 * @brief   Address registration option format
 * @extends ndp_opt_t
 *
 * @see <a href="https://tools.ietf.org/html/rfc6775#section-4.1">
 *          RFC 6775, section 4.1
 *      </a>
 */
typedef struct __attribute__((packed)) {
    uint8_t type;           /**< option type */
    uint8_t len;            /**< length in units of 8 octets */
    uint8_t status;         /**< indicates registration status in NA response;
                             *   must be 0 in NS messages */
    uint8_t resv[3];        /**< reserved fields */
    network_uint16_t ltime; /**< registration lifetime */
    eui64_t eui64;          /**< EUI-64 */
} sixlowpan_nd_opt_ar_t;

/**
 * @brief   6LoWPAN context option format
 * @extends ndp_opt_t
 *
 * @see <a href="https://tools.ietf.org/html/rfc6775#section-4.2">
 *          RFC 6775, section 4.2
 *      </a>
 */
typedef struct __attribute__((packed)) {
    uint8_t type;           /**< option type */
    uint8_t len;            /**< length in units of 8 octets */
    uint8_t ctx_len;        /**< context length */
    uint8_t resv_c_cid;     /**< 3-bit reserved, 1-bit c flag, 4 bit CID */
    network_uint16_t resv;  /**< reserved field */
    network_uint16_t ltime; /**< valid lifetime */
} sixlowpan_nd_opt_6ctx_t;

/**
 * @brief   Authoritative border router option format
 * @extends ndp_opt_t
 *
 * @see <a href="https://tools.ietf.org/html/rfc6775#section-4.3">
 *          RFC 6775, section 4.3
 *      </a>
 */
typedef struct __attribute__((packed)) {
    uint8_t type;           /**< option type */
    uint8_t len;            /**< length in units of 8 octets */
    network_uint16_t vlow;  /**< version low */
    network_uint16_t vhigh; /**< version high */
    network_uint16_t ltime; /**< valid lifetime */
    ipv6_addr_t braddr;     /**< 6LoWPAN border router address */
} sixlowpan_nd_opt_abr_t;

/**
 * @brief   Checks if a 6LoWPAN context in an 6LoWPAN context option is
 *          valid for compression.
 *
 * @param[in] ctx_opt   A 6LoWPAN context option.
 *
 * @return  true, if C bit is set in @p ctx_opt.
 * @return  false, if C bit is unset in @p ctx_opt.
 */
static inline bool sixlowpan_nd_opt_6ctx_is_comp(const sixlowpan_nd_opt_6ctx_t *ctx_opt)
{
    return (bool)(ctx_opt->resv_c_cid & SIXLOWPAN_ND_OPT_6CTX_FLAGS_C);
}

/**
 * @brief   Gets a CID (context identifier) from a 6LoWPAN context option.
 *
 * @param[in] ctx_opt   A 6LoWPAN context option.
 *
 * @return  The CID in the 6LoWPAN context option @p ctx_opt.
 */
static inline uint8_t sixlowpan_nd_opt_6ctx_get_cid(const sixlowpan_nd_opt_6ctx_t *ctx_opt)
{
    return (ctx_opt->resv_c_cid & SIXLOWPAN_ND_OPT_6CTX_FLAGS_CID_MASK);
}

/**
 * @brief   Sets a CID (context identifier) for a 6LoWPAN context option.
 *
 * @param[out] ctx_opt  A 6LoWPAN context option.
 * @param[in] cid       The CID for the 6LoWPAN context option.
 */
static inline void sixlowpan_nd_opt_6ctx_set_cid(sixlowpan_nd_opt_6ctx_t *ctx_opt, uint8_t cid)
{
    ctx_opt->resv_c_cid &= ~SIXLOWPAN_ND_OPT_6CTX_FLAGS_CID_MASK;
    ctx_opt->resv_c_cid |= (SIXLOWPAN_ND_OPT_6CTX_FLAGS_CID_MASK & cid);
}


#ifdef __cplusplus
}
#endif

#endif /* SIXLOWPAN_ND_H_ */
/** @} */
