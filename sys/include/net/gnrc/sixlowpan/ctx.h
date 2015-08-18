/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_gnrc_sixlowpan    6LoWPAN
 * @ingroup     net
 *
 * @defgroup    net_gnrc_sixlowpan_ctx    Contexts for 6LoWPAN address compression
 * @ingroup     net_gnrc_sixlowpan
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
#ifndef GNRC_SIXLOWPAN_CTX_H_
#define GNRC_SIXLOWPAN_CTX_H_

#include <inttypes.h>
#include <stdbool.h>

#include "net/ipv6/addr.h"

#ifdef __cplusplus
extern "C" {
#endif

#define GNRC_SIXLOWPAN_CTX_SIZE (16)    /**< maximum number of entries in
                                         *   context buffer */

/**
 * @{
 * @name    Context flags.
 * @brief   Flags to set in gnrc_sixlowpan_ctx_t::flags_id.
 */
#define GNRC_SIXLOWPAN_CTX_FLAGS_CID_MASK   (0x0f)  /**< mask for the Context ID. */
#define GNRC_SIXLOWPAN_CTX_FLAGS_COMP       (0x10)  /**< Use context for compression */
/**
 * @}
 */

/**
 * @brief   Entry in the 6LoWPAN context buffer.
 */
typedef struct {
    ipv6_addr_t prefix;     /**< The prefix associated to this context. */
    uint8_t prefix_len;     /**< Length of gnrc_sixlowpan_ctx_t::prefix in bit. */
    /**
     * @brief   4-bit flags, 4-bit Context ID.
     *
     * @note    This needs to be here to easily translate prefixes to
     *          ID.
     *
     * @details The flags are defined as above.
     */
    uint8_t flags_id;
    /**
     * @brief   Lifetime in minutes this context is valid.
     *
     * @see     <a href="http://tools.ietf.org/html/rfc6775#section-4.2">
     *              6LoWPAN Context Option
     *          </a>
     */
    uint16_t ltime;
} gnrc_sixlowpan_ctx_t;

/**
 * @brief   Gets a context matching the given IPv6 address best with its prefix.
 *
 * @param[in] addr  An IPv6 address.
 *
 * @return  The context associated with the best prefix for @p addr.
 * @return  NULL if there is no such context.
 */
gnrc_sixlowpan_ctx_t *gnrc_sixlowpan_ctx_lookup_addr(const ipv6_addr_t *addr);

/**
 * @brief   Gets context by ID.
 *
 * @param[in] id    A context ID.
 *
 * @return  The context associated with @p id.
 * @return  NULL if there is no such context.
 */
gnrc_sixlowpan_ctx_t *gnrc_sixlowpan_ctx_lookup_id(uint8_t id);

/**
 * @brief   Updates (or adds if currently not registered) a context
 *
 * @param[in] id            The ID for the context.
 *                          Must be < @ref GNRC_SIXLOWPAN_CTX_SIZE.
 * @param[in] prefix        The prefix for the context.
 * @param[in] prefix_len    Length of @p prefix in bits. Must be > 0.
 * @param[in] ltime         New lifetime of the context. @p comp will be
 *                          implicitly set to `false` if @p ltime == 0.
 * @param[in] comp          Use for compression if true, do not use for
 *                          compression, but still for decompression if false.
 *
 * @return  The new context on success.
 * @return  NULL, on error or on removal.
 */
gnrc_sixlowpan_ctx_t *gnrc_sixlowpan_ctx_update(uint8_t id, const ipv6_addr_t *prefix,
                                                uint8_t prefix_len, uint16_t ltime,
                                                bool comp);

/**
 * @brief   Removes context.
 *
 * @param[in] id    A context ID.
 */
static inline void gnrc_sixlowpan_ctx_remove(uint8_t id)
{
    gnrc_sixlowpan_ctx_lookup_id(id)->prefix_len = 0;
}

#ifdef TEST_SUITES
/**
 * @brief   Resets the whole context buffer.
 */
void gnrc_sixlowpan_ctx_reset(void);
#endif

#ifdef __cplusplus
}
#endif

#endif /* GNRC_SIXLOWPAN_CTX_H_ */
/** @} */
