/*-
 * Copyright 2005 Colin Percival
 * Copyright 2013 Christian Mehlis & René Kijewski
 * Copyright 2016 Martin Landsmann <martin.landsmann@haw-hamburg.de>
 * Copyright 2016 OTA keys S.A.
 * Copyright 2020 HAW Hamburg
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 * $FreeBSD: src/lib/libmd/sha256.h,v 1.1.2.1 2005/06/24 13:32:25 cperciva Exp $
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
