/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_ng_sixlowpan    6LoWPAN
 * @ingroup     net
 *
 * @defgroup    net_ng_sixlowpan_ctx    Contexts for 6LoWPAN address compression
 * @ingroup     net_ng_sixlowpan
 * @brief       Context buffer for stateful 6LoWPAN address compression
 * @see         <a href="https://tools.ietf.org/html/rfc6282#section-3.1.2">
 *                  RFC 6282, section 3.1.2
 *              </a>
 * @see         <a href="http://tools.ietf.org/html/rfc6775#section-4.2">
 *                  RFC 6775, section 4.2
 *              </a>
 * @{
 *
 * @file
 * @brief   Context buffer definitions
 *
 * @author  Martine Lenders <mlenders@inf.fu-berlin.de>
 */
#ifndef NG_SIXLOWPAN_CTX_H_
#define NG_SIXLOWPAN_CTX_H_

#include <inttypes.h>

#include "net/ng_ipv6/addr.h"

#ifdef __cplusplus
extern "C" {
#endif

#define NG_SIXLOWPAN_CTX_SIZE   (16)    /**< maximum number of entries in
                                         *   context buffer */

/**
 * @brief   Entry in the 6LoWPAN context buffer.
 */
typedef struct {
    ng_ipv6_addr_t prefix;  /**< The prefix associated to this context. */
    uint8_t prefix_len;     /**< Length of ng_sixlowpan_ctx_t::prefix in bit. */
    /**
     * @brief   4-bit Context ID.
     *
     * @note    This needs to be here to easily translate prefixes to
     *          ID.
     */
    uint8_t id;
    /**
     * @brief   Lifetime in minutes this context is valid.
     *
     * @see     <a href="http://tools.ietf.org/html/rfc6775#section-4.2">
     *              6LoWPAN Context Option
     *          </a>
     */
    uint16_t ltime;
} ng_sixlowpan_ctx_t;

/**
 * @brief   Gets a context matching the given IPv6 address best with its prefix.
 *
 * @param[in] addr  An IPv6 address.
 *
 * @return  The context associated with the best prefix for @p addr.
 * @return  NULL if there is no such context.
 */
ng_sixlowpan_ctx_t *ng_sixlowpan_ctx_lookup_addr(const ng_ipv6_addr_t *addr);

/**
 * @brief   Gets context by ID.
 *
 * @param[in] id    A context ID.
 *
 * @return  The context associated with @p id.
 * @return  NULL if there is no such context.
 */
ng_sixlowpan_ctx_t *ng_sixlowpan_ctx_lookup_id(uint8_t id);

/**
 * @brief   Updates (or adds if currently not registered) a context
 *
 * @param[in] id            The ID for the context.
 *                          Must be < @ref NG_SIXLOWPAN_CTX_SIZE.
 * @param[in] prefix        The prefix for the context.
 * @param[in] prefix_len    Length of @p prefix in bits. Must be > 0, when
 *                          @p ltime > 0.
 * @param[in] ltime         New lifetime of the context. The context will
 *                          be removed if 0.
 *
 * @return  The new context on success.
 * @return  NULL, on error or on removal.
 */
ng_sixlowpan_ctx_t *ng_sixlowpan_ctx_update(uint8_t id, const ng_ipv6_addr_t *prefix,
                                            uint8_t prefix_len, uint16_t ltime);


#ifdef TEST_SUITES
/**
 * @brief   Resets the whole context buffer.
 */
void ng_sixlowpan_ctx_reset(void);
#endif

#ifdef __cplusplus
}
#endif

#endif /* NG_SIXLOWPAN_CTX_H_ */
/** @} */
