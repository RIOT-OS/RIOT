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

#ifndef HASHES_SHA2XX_COMMON_H
#define HASHES_SHA2XX_COMMON_H

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
    /** processed bytes counter */
    uint32_t count[2];
    /** data buffer */
    unsigned char buf[64];
} sha2xx_context_t;

/**
 * @brief    Elementary functions used by SHA2XX
 * @{
 */
#define Ch(x, y, z) ((x & (y ^ z)) ^ z)
#define Maj(x, y, z)    ((x & (y | z)) | (y & z))
#define SHR(x, n)   (x >> n)
#define ROTR(x, n)  ((x >> n) | (x << (32 - n)))
#define S0(x)       (ROTR(x, 2) ^ ROTR(x, 13) ^ ROTR(x, 22))
#define S1(x)       (ROTR(x, 6) ^ ROTR(x, 11) ^ ROTR(x, 25))
#define s0(x)       (ROTR(x, 7) ^ ROTR(x, 18) ^ SHR(x, 3))
#define s1(x)       (ROTR(x, 17) ^ ROTR(x, 19) ^ SHR(x, 10))
/** @} */

/** @brief SHA-224 and SHA-256 Constants */
static const uint32_t K[64] = {
    0x428a2f98, 0x71374491, 0xb5c0fbcf, 0xe9b5dba5,
    0x3956c25b, 0x59f111f1, 0x923f82a4, 0xab1c5ed5,
    0xd807aa98, 0x12835b01, 0x243185be, 0x550c7dc3,
    0x72be5d74, 0x80deb1fe, 0x9bdc06a7, 0xc19bf174,
    0xe49b69c1, 0xefbe4786, 0x0fc19dc6, 0x240ca1cc,
    0x2de92c6f, 0x4a7484aa, 0x5cb0a9dc, 0x76f988da,
    0x983e5152, 0xa831c66d, 0xb00327c8, 0xbf597fc7,
    0xc6e00bf3, 0xd5a79147, 0x06ca6351, 0x14292967,
    0x27b70a85, 0x2e1b2138, 0x4d2c6dfc, 0x53380d13,
    0x650a7354, 0x766a0abb, 0x81c2c92e, 0x92722c85,
    0xa2bfe8a1, 0xa81a664b, 0xc24b8b70, 0xc76c51a3,
    0xd192e819, 0xd6990624, 0xf40e3585, 0x106aa070,
    0x19a4c116, 0x1e376c08, 0x2748774c, 0x34b0bcb5,
    0x391c0cb3, 0x4ed8aa4a, 0x5b9cca4f, 0x682e6ff3,
    0x748f82ee, 0x78a5636f, 0x84c87814, 0x8cc70208,
    0x90befffa, 0xa4506ceb, 0xbef9a3f7, 0xc67178f2,
};

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
#endif /* HASHES_SHA2XX_COMMON_H */
