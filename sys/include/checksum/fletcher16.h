/*
 * Copyright (C) 2015 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup    sys_checksum_fletcher16 Fletcher16
 * @ingroup     sys_checksum
 *
 * @brief       Fletcher16 checksum algorithm
 * @{
 *
 * @file
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 */


#include <stdlib.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Fletcher's 16 bit checksum context struct
 */
typedef struct {
    uint16_t sum1;  /**< First sum of the checksum */
    uint16_t sum2;  /**< Second sum of the checksum */
} fletcher16_ctx_t;

/**
 * @brief Fletcher's 16 bit checksum
 * @anchor fletcher16_full
 *
 * found on
 * http://en.wikipedia.org/w/index.php?title=Fletcher%27s_checksum&oldid=661273016#Optimizations
 *
 * @note the returned sum is never 0
 *
 * @param buf input buffer to hash
 * @param bytes length of buffer, in bytes
 * @return 16 bit sized hash in the interval [1..65535]
 */
uint16_t fletcher16(const uint8_t *buf, size_t bytes);

/**
 * @brief Initialize a fletcher16 context
 *
 * Multi-part version of @ref fletcher16_full.
 *
 * @param[in]   ctx     fletcher16 context to initialize
 */
void fletcher16_init(fletcher16_ctx_t *ctx);

/**
 * @brief Update the fletcher16 context with new data
 *
 * @param[in]   ctx     fletcher16 context
 * @param[in]   data    Data to add to the context
 * @param[in]   len     Length of the data
 */
void fletcher16_update(fletcher16_ctx_t *ctx, const uint8_t *data, size_t len);

/**
 * @brief Finalize the checksum operation and return the checksum
 *
 * @param[in]   ctx     fletcher16 context
 *
 * @return              Checksum of the data
 */
uint16_t fletcher16_finish(fletcher16_ctx_t *ctx);

#ifdef __cplusplus
}
#endif


/** @} */
