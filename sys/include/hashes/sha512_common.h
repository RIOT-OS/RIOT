/*
 * Copyright (C) 2023 TU Dresden
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup    sys_hashes_sha512_common SHA-512 common
 * @ingroup     sys_hashes_unkeyed
 * @brief       Implementation of common functionality for SHA-512 hashing function
 * @{
 *
 * @file
 * @brief       Common definitions for the SHA-512 hash function
 *
 * @author      Mikolai Gütschow <mikolai.guetschow@tu-dresden.de>
 */


#include <string.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief    Structure to hold the SHA-512 context.
 */
typedef struct {
    /** global state */
    uint64_t state[8];
    /** processed bits counter */
    uint64_t count[2];
    /** data buffer */
    unsigned char buf[128];
} sha512_common_context_t;

/**
 * @brief SHA-512 initialization.  Begins a SHA-512 operation.
 *
 * @param ctx  sha512_common_context_t handle to init, must not be NULL
 */
void sha512_common_pad(sha512_common_context_t *ctx);

/**
 * @brief Add bytes into the hash
 *
 * @param ctx      sha512_common_context_t handle to use, must not be NULL
 * @param[in] data Input data
 * @param[in] len  Length of @p data
 */
void sha512_common_update(sha512_common_context_t *ctx, const void *data, size_t len);

/**
 * @brief SHA-512 finalization.  Pads the input data, exports the hash value,
 * and clears the context state.
 *
 * @param ctx           sha512_common_context_t handle to use, must not be NULL
 * @param[out] digest   resulting digest, this is the hash of all the bytes
 * @param[in] dig_len   Length of @p digest
 */
void sha512_common_final(sha512_common_context_t *ctx, void *digest, size_t dig_len);

#ifdef __cplusplus
}
#endif

/** @} */
