/*
 * Copyright (C) 2024 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_riotconf_hdr RIOT configuration header API
 * @ingroup     sys
 * @{
 *
 * @file
 * @brief       RIOT configuration partition header
 *
 * @author      Fabian Hüßler <fabian.huessler@ml-pa.com>
 *
 * @}
 */

#ifndef RIOTCONF_HDR_H
#define RIOTCONF_HDR_H

#include <stddef.h>
#include <stdint.h>

#include "checksum/fletcher32.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  Magic number for riotconf_header "c0nf"
 */
#define RIOTCONF_MAGIC          0x63306e66

/**
 * @brief   RIOT configuration status flags typedef for extensibility
 */
typedef uint32_t riotconf_hdr_state_t;

/**
 * @brief   RIOT configuration slot typedef for extensibility
 */
typedef int riotconf_slot_t;

/**
 * @brief   RIOT configuration status flags
 */
enum {
    /**
     * @brief   Configuration checksum has been validated
     */
    RIOTCONF_HDR_STATE_CHECKSUM_VALIDATED = 0x01u,
    /**
     * @brief   Configuration values have been verified
     */
    RIOTCONF_HDR_STATE_CONFIGURATION_VERIFIED = 0x02u,
};

/**
 * @brief   RIOT configuration header
 */
typedef struct riotconf_hdr {
    uint32_t magic;                 /**< magic number for fast rejection */
    uint32_t sequence;              /**< anti rollback counter */
    uint32_t version;               /**< version to check compatibility */
    uint32_t size;                  /**< data size */
    uint32_t checksum;              /**< checksum computed over header and data */
    riotconf_hdr_state_t state;     /**< state flags */
} riotconf_hdr_t;

/**
 * @brief   RIOT configuration header checksum context
 */
typedef struct riotconf_hdr_checksum_ctx {
    fletcher32_ctx_t ctx;           /**< internal fletcher context */
} riotconf_hdr_checksum_ctx_t;

/**
 * @brief   Initialize a checksum context and prepare a riotconf header for calculation calculation
 *
 * @param[out]      ctx         Checksum context to initialize
 * @param[in, out]  hdr         Header to prepare for checksum calculation
 */
void riotconf_hdr_checksum_init(riotconf_hdr_checksum_ctx_t *ctx, riotconf_hdr_t *hdr);

/**
 * @brief   Update a checksum context with data
 *
 * @pre    riotconf_hdr_checksum_init() has been called on @p ctx
 *
 * @param[in, out]  ctx         Checksum context to update
 * @param[in]       data        Data to update the checksum with
 * @param[in]       size        Size of the data
 */
void riotconf_hdr_checksum_update(riotconf_hdr_checksum_ctx_t *ctx, void *data, size_t size);

/**
 * @brief   Finalize a checksum context and write the checksum to the header
 *
 * @pre    riotconf_hdr_checksum_init() has been called on @p ctx
 *
 * @param[in, out]  ctx         Checksum context to finalize
 * @param[in, out]  hdr         Header to write the checksum to
 */
void riotconf_hdr_checksum_final(riotconf_hdr_checksum_ctx_t *ctx, riotconf_hdr_t *hdr);

/**
 * @brief   Validate a RIOT configuration header
 *
 * @param[in]   hdr     Header to validate
 *
 * @return  0 on success
 * @return  -EINVAL on magic number mismatch
 */
int riotconf_hdr_validate(riotconf_hdr_t *hdr);

/**
 * @brief   Convert a RIOT configuration header from host to network byte order
 *
 * @param[in,out]   hdr     Header to convert
 */
void riotconf_hdr_hton(riotconf_hdr_t *hdr);

/**
 * @brief   Convert a RIOT configuration header from network to host byte order
 *
 * @param[in,out]   hdr     Header to convert
 */
void riotconf_hdr_ntoh(riotconf_hdr_t *hdr);

#ifdef __cplusplus
}
#endif
#endif /* RIOTCONF_HDR_H */
