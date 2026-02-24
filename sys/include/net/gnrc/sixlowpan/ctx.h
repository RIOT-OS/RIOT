/*
 * SPDX-FileCopyrightText: 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
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

#include <inttypes.h>
#include <stdbool.h>

#include "net/ipv6/addr.h"
#include "timex.h"
#include "modules.h"

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
    if (IS_USED(MODULE_GNRC_SIXLOWPAN_CTX)) {
        gnrc_sixlowpan_ctx_lookup_id(id)->prefix_len = 0;
    }
}

/**
 * @brief   Check if a prefix matches a compression context
 *
 * @param[in] ctx        The compression context
 * @param[in] prefix     IPv6 prefix
 * @param[in] prefix_len Length of the IPv6 prefix
 *
 * @return    true if the prefix matches the compression context.
 */
static inline bool gnrc_sixlowpan_ctx_match(const gnrc_sixlowpan_ctx_t *ctx,
                                            const ipv6_addr_t *prefix, uint8_t prefix_len)
{
    return (ctx != NULL) &&
           (ctx->prefix_len == prefix_len) &&
           (ipv6_addr_match_prefix(&ctx->prefix, prefix) >= prefix_len);
}

/**
 * @brief   Create or update a compression context
 *
 * @param[in] prefix     IPv6 prefix of the compression context
 * @param[in] prefix_len Length of the IPv6 prefix
 * @param[in] valid      Lifetime of the prefix in seconds
 *
 * @return    true if a new compression context was created or an existing context
 *                 was updated.
 *            false if no new context could be added
 */
static inline bool gnrc_sixlowpan_ctx_update_6ctx(const ipv6_addr_t *prefix, uint8_t prefix_len,
                                                  uint32_t valid)
{
    if (!IS_USED(MODULE_GNRC_SIXLOWPAN_CTX)) {
        return false;
    }

    gnrc_sixlowpan_ctx_t *ctx = gnrc_sixlowpan_ctx_lookup_addr(prefix);
    uint8_t cid = 0;

    if (!gnrc_sixlowpan_ctx_match(ctx, prefix, prefix_len)) {
        /* While the context is a prefix match, the defined prefix within the
         * context does not match => use new context */
        ctx = NULL;
    }
    else {
        cid = ctx->flags_id & GNRC_SIXLOWPAN_CTX_FLAGS_CID_MASK;
    }
    /* find first free context ID */
    if (ctx == NULL) {
        while (((ctx = gnrc_sixlowpan_ctx_lookup_id(cid)) != NULL) &&
               !gnrc_sixlowpan_ctx_match(ctx, prefix, prefix_len)) {
            cid++;
        }
    }
    if (cid < GNRC_SIXLOWPAN_CTX_SIZE) {
        return gnrc_sixlowpan_ctx_update(cid, (ipv6_addr_t *)prefix, prefix_len,
                                         valid / (60 * MS_PER_SEC),
                                         true);
    }

    return false;
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

/** @} */
