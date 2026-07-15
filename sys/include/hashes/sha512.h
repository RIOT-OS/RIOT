/*
 * SPDX-FileCopyrightText: 2023 TU Dresden
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    sys_hashes_sha512 SHA-512
 * @ingroup     sys_hashes_unkeyed
 * @brief       Implementation of the SHA-512 hashing function
 * @{
 *
 * @file
 * @brief       Header definitions for the SHA512 hash function
 *
 * @author      Mikolai Gütschow <mikolai.guetschow@tu-dresden.de>
 */

#include <inttypes.h>
#include <stddef.h>

#include "hashes/sha512_common.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Length of SHA512 digests in bytes
 */
#define SHA512_DIGEST_LENGTH (64)

/**
 * @brief 1024 Bit (128 Byte) internally used block size for sha512
 */
#define SHA512_INTERNAL_BLOCK_SIZE (128)

/**
 * @brief Context for cipher operations based on sha512
 */
typedef sha512_common_context_t sha512_context_t;

/**
 * @brief SHA-512 initialization.  Begins a SHA-512 operation.
 *
 * @param ctx  sha512_context_t handle to init, must not be NULL
 */
void sha512_init(sha512_context_t *ctx);

/**
 * @brief Add bytes into the hash
 *
 * @param ctx      sha512_context_t handle to use, must not be NULL
 * @param[in] data Input data
 * @param[in] len  Length of @p data
 */
static inline void sha512_update(sha512_context_t *ctx, const void *data, size_t len)
{
    sha512_common_update(ctx, data, len);
}

/**
 * @brief SHA-512 finalization.  Pads the input data, exports the hash value,
 * and clears the context state.
 *
 * @param ctx           sha512_context_t handle to use, must not be NULL
 * @param[out] digest   pointer to resulting digest, this is the hash of all the bytes.
 *                      Length must be at least SHA512_DIGEST_LENGTH
 */
static inline void sha512_final(sha512_context_t *ctx, void *digest)
{
    sha512_common_final(ctx, digest, SHA512_DIGEST_LENGTH);
}

/**
 * @brief A wrapper function to simplify the generation of a hash. This is
 * useful for generating sha512 for one single buffer in a single step.
 *
 * @param[in] data      pointer to the buffer to generate hash from
 * @param[in] len       length of the buffer
 * @param[out] digest   optional pointer to an array for the result, length must
 *                      be SHA512_DIGEST_LENGTH
 */
void sha512(const void *data, size_t len, void *digest);

#ifdef __cplusplus
}
#endif

/** @} */
