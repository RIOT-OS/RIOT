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
 * @ingroup     sys_hashes
 * @{
 *
 * @file
 * @brief       Common code for SHA2XX hash functions
 *
 * @author      Colin Percival
 * @author      Christian Mehlis
 * @author      Rene Kijewski
 * @author      Martin Landsmann
 * @author      Hermann Lelong
 *
 * @}
 */

#include <stdint.h>
#include <assert.h>

#include "hashes/sha2xx_common.h"

#ifdef __BIG_ENDIAN__
/* Copy a vector of big-endian uint32_t into a vector of bytes */
#define be32enc_vect memcpy

/* Copy a vector of bytes into a vector of big-endian uint32_t */
#define be32dec_vect memcpy
#else /* !__BIG_ENDIAN__ */

/*
 * Encode a length len/4 vector of (uint32_t) into a length len vector of
 * (unsigned char) in big-endian form.  Assumes len is a multiple of 4.
 */
static void be32enc_vect(void *dst_, const void *src_, size_t len)
{
    /* Assert if len is not a multiple of 4 */
    assert(!(len & 3));

    if ((uintptr_t)dst_ % sizeof(uint32_t) == 0 &&
        (uintptr_t)src_ % sizeof(uint32_t) == 0) {
        uint32_t *dst = dst_;
        const uint32_t *src = src_;
        for (size_t i = 0; i < len / 4; i++) {
            dst[i] = __builtin_bswap32(src[i]);
        }
    }
    else {
        uint8_t *dst = dst_;
        const uint8_t *src = src_;
        for (size_t i = 0; i < len; i += 4) {
            dst[i] = src[i + 3];
            dst[i + 1] = src[i + 2];
            dst[i + 2] = src[i + 1];
            dst[i + 3] = src[i];
        }
    }
}

/*
 * Decode a big-endian length len vector of (unsigned char) into a length
 * len/4 vector of (uint32_t).  Assumes len is a multiple of 4.
 */
#define be32dec_vect be32enc_vect

#endif /* __BYTE_ORDER__ != __ORDER_BIG_ENDIAN__ */

/*
 * SHA256 block compression function.  The 256-bit state is transformed via
 * the 512-bit input block to produce a new state.
 */
static void sha2xx_transform(uint32_t *state, const unsigned char block[64])
{
    uint32_t W[64];
    uint32_t S[8];

    /* 1. Prepare message schedule W. */
    be32dec_vect(W, block, 64);
    for (int i = 16; i < 64; i++) {
        W[i] = s1(W[i - 2]) + W[i - 7] + s0(W[i - 15]) + W[i - 16];
    }

    /* 2. Initialize working variables. */
    memcpy(S, state, 32);

    /* 3. Mix. */
    for (int i = 0; i < 64; ++i) {
        uint32_t e = S[(68 - i) % 8], f = S[(69 - i) % 8];
        uint32_t g = S[(70 - i) % 8], h = S[(71 - i) % 8];
        uint32_t t0 = h + S1(e) + Ch(e, f, g) + W[i] + K[i];

        uint32_t a = S[(64 - i) % 8], b = S[(65 - i) % 8];
        uint32_t c = S[(66 - i) % 8], d = S[(67 - i) % 8];
        uint32_t t1 = S0(a) + Maj(a, b, c);

        S[(67 - i) % 8] = d + t0;
        S[(71 - i) % 8] = t0 + t1;
    }

    /* 4. Mix local working variables into global state */
    for (int i = 0; i < 8; i++) {
        state[i] += S[i];
    }
}

static unsigned char PAD[64] = {
    0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

/* Add padding and terminating bit-count. */
void sha2xx_pad(sha2xx_context_t *ctx)
{
    /*
     * Convert length to a vector of bytes -- we do this now rather
     * than later because the length will change after we pad.
     */
    unsigned char len[8];

    be32enc_vect(len, ctx->count, 8);

    /* Add 1--64 bytes so that the resulting length is 56 mod 64 */
    uint32_t r = (ctx->count[1] >> 3) & 0x3f;
    uint32_t plen = (r < 56) ? (56 - r) : (120 - r);
    sha2xx_update(ctx, PAD, (size_t) plen);

    /* Add the terminating bit-count */
    sha2xx_update(ctx, len, 8);
}

/* Add bytes into the hash */
void sha2xx_update(sha2xx_context_t *ctx, const void *data, size_t len)
{
    /* Number of bytes left in the buffer from previous updates */
    uint32_t r = (ctx->count[1] >> 3) & 0x3f;

    /* Convert the length into a number of bits */
    uint32_t bitlen1 = ((uint32_t) len) << 3;
    uint32_t bitlen0 = ((uint32_t) len) >> 29;

    /* Update number of bits */
    if ((ctx->count[1] += bitlen1) < bitlen1) {
        ctx->count[0]++;
    }

    ctx->count[0] += bitlen0;

    /* Handle the case where we don't need to perform any transforms */
    if (len < 64 - r) {
        if (len > 0) {
            memcpy(&ctx->buf[r], data, len);
        }
        return;
    }

    /* Finish the current block */
    const unsigned char *src = data;

    memcpy(&ctx->buf[r], src, 64 - r);
    sha2xx_transform(ctx->state, ctx->buf);
    src += 64 - r;
    len -= 64 - r;

    /* Perform complete blocks */
    while (len >= 64) {
        sha2xx_transform(ctx->state, src);
        src += 64;
        len -= 64;
    }

    /* Copy left over data into buffer */
    memcpy(ctx->buf, src, len);
}

/*
 * SHA-224 finalization.  Pads the input data, exports the hash value,
 * and clears the context state.
 */
void sha2xx_final(sha2xx_context_t *ctx, void *dst, size_t dig_len)
{
    /* Add padding */
    sha2xx_pad(ctx);

    /* Write the hash */
    be32enc_vect(dst, ctx->state, dig_len);

    /* Clear the context state */
    memset((void *) ctx, 0, sizeof(*ctx));
}
