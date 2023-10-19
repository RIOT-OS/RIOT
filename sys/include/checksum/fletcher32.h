/*
 * Copyright (C) 2015 Eistec AB
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_checksum_fletcher32 Fletcher32
 * @ingroup     sys_checksum
 *
 * @brief       Fletcher32 checksum algorithm
 * @{
 *
 * @file
 * @author      Joakim Nohlgård <joakim.nohlgard@eistec.se>
 * @author      Koen Zandberg <koen@bergzand.net>
 */

#ifndef CHECKSUM_FLETCHER32_H
#define CHECKSUM_FLETCHER32_H

#include <stdlib.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Fletcher's 32 bit checksum context struct
 */
typedef struct {
    uint32_t sum1;  /**< First sum of the checksum */
    uint32_t sum2;  /**< Second sum of the checksum */
} fletcher32_ctx_t;

/**
 * @brief Fletcher's 32 bit checksum
 *
 * found on
 * http://en.wikipedia.org/w/index.php?title=Fletcher%27s_checksum&oldid=661273016#Optimizations
 *
 * @note the returned sum is never 0
 * @note pay attention to the @p words parameter buffer containing 16 bit words, not bytes.
 *
 * @param buf input buffer to hash
 * @param words length of buffer, in 16 bit words
 * @return 32 bit sized hash in the interval [1..2^32]
 */
uint32_t fletcher32(const uint16_t *buf, size_t words);

/**
 * @brief Initialize a fletcher32 context
 *
 * Multi-part version of @ref fletcher32.
 *
 * @param[in]   ctx     fletcher32 context to initialize
 */
void fletcher32_init(fletcher32_ctx_t *ctx);

/**
 * @brief Incrementally update the fletcher32 context with new data. Can be an arbitrary amount of
 * times with new data to checksum.
 *
 * @note @p words is the number of 16 bit words in the buffer
 * @note @p data should contain an integer number of 16 bit words
 *
 * @param[in]   ctx     fletcher32 context
 * @param[in]   data    Data to add to the context
 * @param[in]   words   Length of the data in 16 bit words
 */
void fletcher32_update(fletcher32_ctx_t *ctx, const void *data, size_t words);

/**
 * @brief Finalize the checksum operation and return the checksum
 *
 * @param[in]   ctx     fletcher32 context
 *
 * @return 32 bit sized hash in the interval [1..2^32]
 */
uint32_t fletcher32_finish(fletcher32_ctx_t *ctx);

#ifdef __cplusplus
}
#endif

#endif /* CHECKSUM_FLETCHER32_H */

/** @} */
