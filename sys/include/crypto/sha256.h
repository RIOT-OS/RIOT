/*-
 * Copyright 2005 Colin Percival
 * Copyright 2013 Christian Mehlis & René Kijewski
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
 * @ingroup     sys_crypto
 * @{
 *
 * @file
 * @brief       Header definitions for the SHA256 hash function
 *
 * @author      Colin Percival
 * @author      Christian Mehlis
 * @author      Rene Kijewski
 */

#ifndef _SHA256_H_
#define _SHA256_H_

#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

#define SHA256_DIGEST_LENGTH 32

/**
 * @brief Context for ciper operatins based on sha256
 */
typedef struct {
    /** global state */
    uint32_t state[8];
    /** processed bytes counter */
    uint32_t count[2];
    /** data buffer */
    unsigned char buf[64];
} sha256_context_t;

/**
 * @brief SHA-256 initialization.  Begins a SHA-256 operation.
 *
 * @param ctx  sha256_context_t handle to init
 */
void sha256_init(sha256_context_t *ctx);

/**
 * @brief Add bytes into the hash
 *
 * @param ctx  sha256_context_t handle to use
 * @param in   pointer to the input buffer
 * @param len  length of the buffer
 */
void sha256_update(sha256_context_t *ctx, const void *in, size_t len);

/**
 * @brief SHA-256 finalization.  Pads the input data, exports the hash value,
 * and clears the context state.
 *
 * @param digest resulting digest, this is the hash of all the bytes
 * @param ctx    sha256_context_t handle to use
 */
void sha256_final(unsigned char digest[32], sha256_context_t *ctx);

/**
 * @brief A wrapper function to simplify the generation of a hash, this is
 * usefull for generating sha256 for one buffer
 *
 * @param d pointer to the buffer to generate hash from
 * @param n length of the buffer
 * @param md optional pointer to an array for the result, length must be
 *           SHA256_DIGEST_LENGTH
 *           if md == NULL, one static buffer is used
 */
unsigned char *sha256(const unsigned char *d, size_t n, unsigned char *md);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* _SHA256_H_ */
