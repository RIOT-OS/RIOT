/*
 * SPDX-FileCopyrightText: 2005 Colin Percival
 * SPDX-FileCopyrightText: 2013 Christian Mehlis & René Kijewski
 * SPDX-FileCopyrightText: 2016 Martin Landsmann <martin.landsmann@haw-hamburg.de>
 * SPDX-FileCopyrightText: 2016 OTA keys S.A.
 * SPDX-FileCopyrightText: 2020 HAW Hamburg
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

/**
 * @defgroup    sys_hashes_sha224 SHA-224
 * @ingroup     sys_hashes_unkeyed
 * @brief       Implementation of the SHA-224 hashing function
 * @{
 *
 * @file
 * @brief       Header definitions for the SHA224 hash function
 *
 * @author      Colin Percival
 * @author      Christian Mehlis
 * @author      Rene Kijewski
 * @author      Hermann Lelong
 * @author      Peter Kietzmann
 */

#include <inttypes.h>
#include <stddef.h>

#include "hashes/sha2xx_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Length of SHA224 digests in bytes
 */
#define SHA224_DIGEST_LENGTH (28)

/**
 * @brief 512 Bit (64 Byte) internally used block size for sha224
 */
#define SHA224_INTERNAL_BLOCK_SIZE (64)

/**
 * @brief Context for cipher operations based on sha224
 */
typedef sha2xx_context_t sha224_context_t;

/**
 * @brief SHA-224 initialization.  Begins a SHA-224 operation.
 *
 * @param ctx  sha224_context_t handle to init
 */
void sha224_init(sha224_context_t *ctx);

/**
 * @brief Add bytes into the hash
 *
 * @param ctx      sha224_context_t handle to use
 * @param[in] data Input data
 * @param[in] len  Length of @p data
 */
static inline void sha224_update(sha224_context_t *ctx, const void *data, size_t len)
{
    sha2xx_update(ctx, data, len);
}

/**
 * @brief SHA-224 finalization.  Pads the input data, exports the hash value,
 * and clears the context state.
 *
 * @param ctx    sha224_context_t handle to use
 * @param digest resulting digest, this is the hash of all the bytes
 */
static inline void sha224_final(sha224_context_t *ctx, void *digest)
{
    sha2xx_final(ctx, digest, SHA224_DIGEST_LENGTH);
}

/**
 * @brief A wrapper function to simplify the generation of a hash, this is
 * useful for generating sha224 for one buffer
 *
 * @param[in] data   pointer to the buffer to generate hash from
 * @param[in] len    length of the buffer
 * @param[out] digest Pointer to an array for the result, length must
 *                    be SHA224_DIGEST_LENGTH
 */
void sha224(const void *data, size_t len, void *digest);

#ifdef __cplusplus
}
#endif

/** @} */
