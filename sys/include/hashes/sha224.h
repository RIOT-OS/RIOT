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

#ifndef HASHES_SHA224_H
#define HASHES_SHA224_H

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
 * @param[out] digest optional pointer to an array for the result, length must
 *                    be SHA224_DIGEST_LENGTH
 *                    if digest == NULL, one static buffer is used
 */
void *sha224(const void *data, size_t len, void *digest);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* HASHES_SHA224_H */
