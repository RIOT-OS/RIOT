/*-
 * Copyright 2005 Colin Percival
 * Copyright 2013 Christian Mehlis & René Kijewski
 * Copyright 2016 Martin Landsmann <martin.landsmann@haw-hamburg.de>
 * Copyright 2016 OTA keys S.A.
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
 * $FreeBSD: src/lib/libmd/sha256c.c,v 1.2 2006/01/17 15:35:56 phk Exp $
 */

/**
 * @ingroup     sys_hashes
 * @{
 *
 * @file
 * @brief       SHA256 hash function implementation
 *
 * @author      Colin Percival
 * @author      Christian Mehlis
 * @author      Rene Kijewski
 * @author      Martin Landsmann
 * @author      Hermann Lelong
 *
 * @}
 */

#include <string.h>
#include <assert.h>

#include "hashes/sha256.h"
#include "hashes/sha2xx_common.h"

/* SHA-256 initialization.  Begins a SHA-256 operation. */
void sha256_init(sha256_context_t *ctx)
{
    /* Zero bits processed so far */
    ctx->count[0] = ctx->count[1] = 0;

    /* Magic initialization constants */
    ctx->state[0] = 0x6A09E667;
    ctx->state[1] = 0xBB67AE85;
    ctx->state[2] = 0x3C6EF372;
    ctx->state[3] = 0xA54FF53A;
    ctx->state[4] = 0x510E527F;
    ctx->state[5] = 0x9B05688C;
    ctx->state[6] = 0x1F83D9AB;
    ctx->state[7] = 0x5BE0CD19;
}

void *sha256(const void *data, size_t len, void *digest)
{
    sha256_context_t c;
    static unsigned char m[SHA256_DIGEST_LENGTH];

    if (digest == NULL) {
        digest = m;
    }

    sha256_init(&c);
    sha2xx_update(&c, data, len);
    sha256_final(&c, digest);

    return digest;
}

void hmac_sha256_init(hmac_context_t *ctx, const void *key, size_t key_length)
{
    unsigned char k[SHA256_INTERNAL_BLOCK_SIZE];

    memset((void *)k, 0x00, SHA256_INTERNAL_BLOCK_SIZE);

    if (key_length > SHA256_INTERNAL_BLOCK_SIZE) {
        sha256(key, key_length, k);
    }
    else {
        memcpy((void *)k, key, key_length);
    }

    /*
     * create the inner and outer keypads
     * rising hamming distance enforcing i_* and o_* are distinct
     * in at least one bit
     */
    unsigned char o_key_pad[SHA256_INTERNAL_BLOCK_SIZE];
    unsigned char i_key_pad[SHA256_INTERNAL_BLOCK_SIZE];

    for (size_t i = 0; i < SHA256_INTERNAL_BLOCK_SIZE; ++i) {
        o_key_pad[i] = 0x5c ^ k[i];
        i_key_pad[i] = 0x36 ^ k[i];
    }

    /*
     * Initiate calculation of the inner hash
     * tmp = hash(i_key_pad CONCAT message)
     */
    sha256_init(&ctx->c_in);
    sha2xx_update(&ctx->c_in, i_key_pad, SHA256_INTERNAL_BLOCK_SIZE);

    /*
     * Initiate calculation of the outer hash
     * result = hash(o_key_pad CONCAT tmp)
     */
    sha256_init(&ctx->c_out);
    sha2xx_update(&ctx->c_out, o_key_pad, SHA256_INTERNAL_BLOCK_SIZE);

}

void hmac_sha256_update(hmac_context_t *ctx, const void *data, size_t len)
{
    sha2xx_update(&ctx->c_in, data, len);
}

void hmac_sha256_final(hmac_context_t *ctx, void *digest)
{
    unsigned char tmp[SHA256_DIGEST_LENGTH];

    static unsigned char m[SHA256_DIGEST_LENGTH];

    if (digest == NULL) {
        digest = m;
    }

    sha256_final(&ctx->c_in, tmp);
    sha2xx_update(&ctx->c_out, tmp, SHA256_DIGEST_LENGTH);
    sha256_final(&ctx->c_out, digest);
}

const void *hmac_sha256(const void *key, size_t key_length,
                        const void *data, size_t len, void *digest)
{

    hmac_context_t ctx;

    hmac_sha256_init(&ctx, key, key_length);
    hmac_sha256_update(&ctx,data, len);
    hmac_sha256_final(&ctx, digest);

    return digest;
}

/**
 * @brief helper to compute sha256 inplace for the given buffer
 *
 * @param[in, out] element the buffer to compute a sha256 and store it back to it
 *
 */
static inline void sha256_inplace(unsigned char element[SHA256_DIGEST_LENGTH])
{
    sha256_context_t ctx;

    sha256_init(&ctx);
    sha2xx_update(&ctx, element, SHA256_DIGEST_LENGTH);
    sha256_final(&ctx, element);
}

void *sha256_chain(const void *seed, size_t seed_length,
                   size_t elements, void *tail_element)
{
    unsigned char tmp_element[SHA256_DIGEST_LENGTH];

    /* assert if no sha256-chain can be created */
    assert(elements >= 2);

    /* 1st iteration */
    sha256(seed, seed_length, tmp_element);

    /* perform consecutive iterations minus the first one */
    for (size_t i = 0; i < (elements - 1); ++i) {
        sha256_inplace(tmp_element);
    }

    /* store the result */
    memcpy(tail_element, tmp_element, SHA256_DIGEST_LENGTH);

    return tail_element;
}

void *sha256_chain_with_waypoints(const void *seed,
                                  size_t seed_length,
                                  size_t elements,
                                  void *tail_element,
                                  sha256_chain_idx_elm_t *waypoints,
                                  size_t *waypoints_length)
{
    /* assert if no sha256-chain can be created */
    assert(elements >= 2);

    /* assert to prevent division by 0 */
    assert(*waypoints_length > 0);

    /* assert if no waypoints can be created */
    assert(*waypoints_length > 1);

    /* if we have enough space we store the whole chain */
    if (*waypoints_length >= elements) {
        /* 1st iteration */
        sha256(seed, seed_length, waypoints[0].element);
        waypoints[0].index = 0;

        /* perform consecutive iterations starting at index 1*/
        for (size_t i = 1; i < elements; ++i) {
            sha256_context_t ctx;
            sha256_init(&ctx);
            sha2xx_update(&ctx, waypoints[(i - 1)].element, SHA256_DIGEST_LENGTH);
            sha256_final(&ctx, waypoints[i].element);
            waypoints[i].index = i;
        }

        /* store the result */
        memcpy(tail_element, waypoints[(elements - 1)].element, SHA256_DIGEST_LENGTH);
        *waypoints_length = (elements - 1);

        return tail_element;
    }
    else {
        unsigned char tmp_element[SHA256_DIGEST_LENGTH];
        size_t waypoint_streak = (elements / *waypoints_length);

        /* 1st waypoint iteration */
        sha256(seed, seed_length, tmp_element);
        for (size_t i = 1; i < waypoint_streak; ++i) {
            sha256_inplace(tmp_element);
        }
        memcpy(waypoints[0].element, tmp_element, SHA256_DIGEST_LENGTH);
        waypoints[0].index = (waypoint_streak - 1);

        /* index of the current computed element in the chain */
        size_t index = (waypoint_streak - 1);

        /* consecutive waypoint iterations */
        size_t j = 1;
        for (; j < *waypoints_length; ++j) {
            for (size_t i = 0; i < waypoint_streak; ++i) {
                sha256_inplace(tmp_element);
                index++;
            }
            memcpy(waypoints[j].element, tmp_element, SHA256_DIGEST_LENGTH);
            waypoints[j].index = index;
        }

        /* store/pass the last used index in the waypoint array */
        *waypoints_length = (j - 1);

        /* remaining iterations down to elements */
        for (size_t i = index; i < (elements - 1); ++i) {
            sha256_inplace(tmp_element);
        }

        /* store the result */
        memcpy(tail_element, tmp_element, SHA256_DIGEST_LENGTH);

        return tail_element;
    }
}

int sha256_chain_verify_element(void *element,
                                size_t element_index,
                                void *tail_element,
                                size_t chain_length)
{
    unsigned char tmp_element[SHA256_DIGEST_LENGTH];

    int delta_count = (chain_length - element_index);

    /* assert if we have an index mismatch */
    assert(delta_count >= 1);

    memcpy((void *)tmp_element, element, SHA256_DIGEST_LENGTH);

    /* perform all consecutive iterations down to tail_element */
    for (int i = 0; i < (delta_count - 1); ++i) {
        sha256_inplace(tmp_element);
    }

    /* return if the computed element equals the tail_element */
    return (memcmp(tmp_element, tail_element, SHA256_DIGEST_LENGTH) != 0);
}
