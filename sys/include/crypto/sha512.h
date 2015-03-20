/*
 * Copyright (C) 2015 Jan Wagner <mail@jwagner.eu>
 * Copyright (C) 2014 Daniel Beer <dlbeer@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_crypto
 * @{
 *
 * @file
 * @brief       Headers for the implementation of the SHA-512 hash function 
 *
 * @author	Jan Wagner <mail@jwagner.eu>
 * @author	Daniel Beer <dlbeer@gmail.com> 
 * @author	NSA
 *
 */

#ifndef __SHA512_H_
#define __SHA512_H_

#include <stdint.h>
#include <stddef.h>
#include <string.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief The SHA-512 state context
 */
typedef struct {
   uint64_t h[8]; /**< 64bit value array */
} sha512_ctx;

/**
 * @name Algorithm constants 
 * @{
 */
#define SHA512_BLOCK_SIZE	128
#define SHA512_HASH_SIZE	64
/** @} */

/**
 * @brief  SHA-512 Key setup
 *
 * @param[in]   ctx         SHA512 context
 */
void sha512_init(sha512_ctx *ctx);

/**
 * @brief  Add bytes to hash and perform SHA512 block compression 
 *
 * @param[in]   ctx         SHA512 context
 * @param[in]   block       Input bytes 
 */
void sha512_block(sha512_ctx *ctx, const uint8_t *block);

/**
 * @brief  SHA-512 finalization 
 *
 * @param[in]   ctx         SHA512 context
 * @param[in]   block       Input bytes 
 * @param[in]   total_size  Total number of input bytes 
 */
void sha512_final(sha512_ctx *ctx, const uint8_t *block, uint64_t total_size);

/**
 * @brief  Read out bytes from SHA-512 state
 *
 * @param[in]   ctx         SHA512 context
 * @param[out]  hash        Hash output pre-allocted array
 * @param[in]   offset      Skip output bytes
 * @param[in]   len         Number of bytes to output         
 */
void sha512_get(const sha512_ctx *ctx, uint8_t *hash, unsigned int offset, unsigned int len);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* __SHA512_H_ */
