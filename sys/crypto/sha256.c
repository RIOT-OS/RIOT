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
 * $FreeBSD: src/lib/libmd/sha256c.c,v 1.2 2006/01/17 15:35:56 phk Exp $
 */

/**
 * @ingroup     sys_crypto
 * @{
 *
 * @file
 * @brief       SHA256 hash function implementation
 *
 * @author      Colin Percival
 * @author      Christian Mehlis
 * @author      Rene Kijewski
 *
 * @}
 */

#include <string.h>

#include "crypto/sha256.h"
#include "board.h"

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
    uint32_t *dst = dst_;
    const uint32_t *src = src_;
    for (size_t i = 0; i < len / 4; i++) {
        dst[i] = __builtin_bswap32(src[i]);
    }
}

/*
 * Decode a big-endian length len vector of (unsigned char) into a length
 * len/4 vector of (uint32_t).  Assumes len is a multiple of 4.
 */
#define be32dec_vect be32enc_vect

#endif /* __BYTE_ORDER__ != __ORDER_BIG_ENDIAN__ */

/* Elementary functions used by SHA256 */
#define Ch(x, y, z) ((x & (y ^ z)) ^ z)
#define Maj(x, y, z)    ((x & (y | z)) | (y & z))
#define SHR(x, n)   (x >> n)
#define ROTR(x, n)  ((x >> n) | (x << (32 - n)))
#define S0(x)       (ROTR(x, 2) ^ ROTR(x, 13) ^ ROTR(x, 22))
#define S1(x)       (ROTR(x, 6) ^ ROTR(x, 11) ^ ROTR(x, 25))
#define s0(x)       (ROTR(x, 7) ^ ROTR(x, 18) ^ SHR(x, 3))
#define s1(x)       (ROTR(x, 17) ^ ROTR(x, 19) ^ SHR(x, 10))

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

/*
 * SHA256 block compression function.  The 256-bit state is transformed via
 * the 512-bit input block to produce a new state.
 */
static void sha256_transform(uint32_t *state, const unsigned char block[64])
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
static void sha256_pad(sha256_context_t *ctx)
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
    sha256_update(ctx, PAD, (size_t) plen);

    /* Add the terminating bit-count */
    sha256_update(ctx, len, 8);
}

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

/* Add bytes into the hash */
void sha256_update(sha256_context_t *ctx, const void *in, size_t len)
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
        memcpy(&ctx->buf[r], in, len);
        return;
    }

    /* Finish the current block */
    const unsigned char *src = in;

    memcpy(&ctx->buf[r], src, 64 - r);
    sha256_transform(ctx->state, ctx->buf);
    src += 64 - r;
    len -= 64 - r;

    /* Perform complete blocks */
    while (len >= 64) {
        sha256_transform(ctx->state, src);
        src += 64;
        len -= 64;
    }

    /* Copy left over data into buffer */
    memcpy(ctx->buf, src, len);
}

/*
 * SHA-256 finalization.  Pads the input data, exports the hash value,
 * and clears the context state.
 */
void sha256_final(unsigned char digest[32], sha256_context_t *ctx)
{
    /* Add padding */
    sha256_pad(ctx);

    /* Write the hash */
    be32enc_vect(digest, ctx->state, 32);

    /* Clear the context state */
    memset((void *) ctx, 0, sizeof(*ctx));
}

unsigned char *sha256(const unsigned char *d, size_t n, unsigned char *md)
{
    sha256_context_t c;
    static unsigned char m[SHA256_DIGEST_LENGTH];

    if (md == NULL) {
        md = m;
    }

    sha256_init(&c);
    sha256_update(&c, d, n);
    sha256_final(md, &c);

    return md;
}
