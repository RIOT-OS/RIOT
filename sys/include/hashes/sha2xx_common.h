/*-
 * SPDX-FileCopyrightText: 2005 Colin Percival
 * SPDX-FileCopyrightText: 2013 Christian Mehlis & René Kijewski
 * SPDX-FileCopyrightText: 2016 Martin Landsmann <martin.landsmann@haw-hamburg.de>
 * SPDX-FileCopyrightText: 2016 OTA keys S.A.
 * SPDX-FileCopyrightText: 2020 HAW Hamburg
 * SPDX-License-Identifier: BSD-2-Clause
 */

#pragma once

/**
 * @defgroup    sys_hashes_sha2xx_common SHA-2xx common
 * @ingroup     sys_hashes_unkeyed
 * @brief       Implementation of common functionality for SHA-224/256 hashing functions
 * @{
 *
 * @file
 * @brief       Common definitions for the SHA-224/256 hash functions
 *
 * @author      Colin Percival
 * @author      Christian Mehlis
 * @author      Rene Kijewski
 * @author      Hermann Lelong
 * @author      Peter Kietzmann
 */

#include <string.h>
#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief    Structure to hold the SHA-2XX context.
 */
typedef struct {
    /** global state */
    uint32_t state[8];
    /** processed bits counter */
    uint32_t count[2];
    /** data buffer */
    unsigned char buf[64];
} sha2xx_context_t;

/**
 * @brief SHA-2XX initialization.  Begins a SHA-2XX operation.
 *
 * @param ctx  sha2xx_context_t handle to init
 */
void sha2xx_pad(sha2xx_context_t *ctx);

/**
 * @brief Add bytes into the hash
 *
 * @param ctx      sha2xx_context_t handle to use
 * @param[in] data Input data
 * @param[in] len  Length of @p data
 */
void sha2xx_update(sha2xx_context_t *ctx, const void *data, size_t len);

/**
 * @brief SHA-2XX finalization.  Pads the input data, exports the hash value,
 * and clears the context state.
 *
 * @param ctx     sha2xx_context_t handle to use
 * @param digest  resulting digest, this is the hash of all the bytes
 * @param dig_len Length of @p digest
 */
void sha2xx_final(sha2xx_context_t *ctx, void *digest, size_t dig_len);

#ifdef __cplusplus
}
#endif

/** @} */
