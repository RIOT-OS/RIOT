/*
 * Copyright (C) 2014 Martin Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for
 * more details.
 */

/**
 * @defgroup    net_sixlowpan_iphc_cbuf Context buffer for 6LoWPAN IP header compression
 * @addtogroup  net_sixlowpan
 * @{
 *
 * @file        iphc_cbuf.h
 * @brief       Context buffer for 6LoWPAN IP header compression
 *
 * @see         <a href="https://tools.ietf.org/html/rfc6282#section-3.1.2">
 *                  RFC 6282, section 3.1.2
 *              </a>
 * @see         <a href="http://tools.ietf.org/html/rfc6775#section-4.2">
 *                  RFC 6775, section 4.2
 *              </a>
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 */


#ifndef __SIXLOWPAN_IPHC_CBUF_H_
#define __SIXLOWPAN_IPHC_CBUF_H_

#include "ipv6.h"

#define SIXLOWPAN_IPHC_CBUF_SIZE (16)

/**
 * @brief   Data type to configure 6LoWPAN contexts
 * @see <a href="http://tools.ietf.org/html/rfc6282#section-3.1">
 *          RFC 6282, section 3.1
 *      </a>
 */
typedef struct {
    uint8_t cid;            /**< Context ID */
    uint8_t prefix_len;     /**< Length of the prefix in bits, if 0 on
                                 NETAPI_CMD_SET the context will be removed. */
    ipv6_addr_t prefix;     /**< The prefix associated to this context */

    /**
     * @brief   Lifetime in minutes this context is valid.
     *
     * @see     <a href="http://tools.ietf.org/html/rfc6775#section-4.2">
     *              6LoWPAN Context Option
     *          </a>
     */
    uint32_t lifetime;
} sixlowpan_iphc_cbuf_t;

/**
 * @brief   Lookup best context for an IPv6 address.
 *
 * @param[in] addr      An IPv6 address.
 *
 * @return  best fitting context for given IPv6 address.
 * @return  NULL, if no fitting context can be found.
 */
sixlowpan_iphc_cbuf_t *sixlowpan_iphc_cbuf_lookup_addr(const ipv6_addr_t *addr);

/**
 * @brief   Lookup best context for a context ID.
 *
 * @param[in] cid       A context ID, must be < 16.
 *
 * @return  best fitting context for given context ID.
 * @return  NULL, if no fitting context can be found.
 */
sixlowpan_iphc_cbuf_t *sixlowpan_iphc_cbuf_lookup_cid(uint8_t cid);

/**
 * @brief   Returns number of currently saved contexts.
 *
 * @return  Number of currently saved contexts.
 */
uint8_t sixlowpan_iphc_cbuf_num(void);

/**
 * @brief   Remove context by context ID.

 * @param[in] cid       A context ID, must be < 16.
 */
void sixlowpan_iphc_cbuf_remove_by_cid(uint8_t cid);

/**
 * @brief   Remove context by best address match.
 *
 * @param[in] addr  An IPv6 address.
 */
static inline void sixlowpan_iphc_cbuf_remove_by_addr(const ipv6_addr_t *addr)
{
    sixlowpan_iphc_cbuf_t *context = sixlowpan_iphc_cbuf_lookup_addr(addr);

    if (context) {
        sixlowpan_iphc_cbuf_remove_by_cid(context->cid);
    }
}

/**
 * @brief   Removes all context with expired lifetime
 */
void sixlowpan_iphc_cbuf_remove_invalid(void);

/**
 * @brief   Update a context
 *
 * @param[in] cid           A context ID, must be < 16.
 * @param[in] prefix        An IPv6 prefix to associate with *cid*.
 * @param[in] prefix_len    Length of prefix in bits. If *prefix_len* is longer
 *                          then 128, it will be set to 128. Must be > 0.
 * @param[in] lifetime      Lifetime the context is valid in minutes. Must be > 0.
 *
 * @return  The updated context on success.
 * @return  NULL, if cid is >= 16, prefix_len is 0, or lifetime is 0.
 */
sixlowpan_iphc_cbuf_t *sixlowpan_iphc_cbuf_update(uint8_t cid, const ipv6_addr_t *prefix,
        uint8_t prefix_len, uint16_t lifetime);

#endif /* __SIXLOWPAN_IPHC_CBUF_H_ */
/**
 * @}
 */
