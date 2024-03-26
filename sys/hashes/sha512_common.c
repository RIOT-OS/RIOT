/*
 * Copyright (C) 2023 TU Dresden
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_hashes_sha512_common SHA-512 common
 * @ingroup     sys_hashes_unkeyed
 * @brief       Implementation of common functionality for SHA-512-based hashing functions
 * @{
 *
 * @file
 * @brief       Common code for SHA-512-based hash functions
 *
 * @author      Mikolai Gütschow <mikolai.guetschow@tu-dresden.de>
 *
 * @}
 */

#include <stdint.h>
#include <assert.h>

#include "hashes/sha512_common.h"

#ifdef __BIG_ENDIAN__
/* Copy a vector of big-endian uint32_t into a vector of bytes */
#define be64enc_vect memcpy

/* Copy a vector of bytes into a vector of big-endian uint32_t */
#define be64dec_vect memcpy
#else /* !__BIG_ENDIAN__ */

/*
 * Encode a length ceil(len/8) vector of (uint64_t) into a length len vector of
 * (unsigned char) in big-endian form.
 */
static void be64enc_vect(void *dst_, const void *src_, size_t len)
{
    size_t i;
    if ((uintptr_t)dst_ % sizeof(uint64_t) == 0 &&
        (uintptr_t)src_ % sizeof(uint64_t) == 0) {
        uint64_t *dst = dst_;
        const uint64_t *src = src_;
        for (i = 0; i < len / 8; i++) {
            dst[i] = __builtin_bswap64(src[i]);
        }
        i *= 8;
    }
    else {
        uint8_t *dst = dst_;
        const uint8_t *src = src_;
        for (i = 0; i < len-7; i += 8) {
            dst[i] = src[i + 7];
            dst[i + 1] = src[i + 6];
            dst[i + 2] = src[i + 5];
            dst[i + 3] = src[i + 4];
            dst[i + 4] = src[i + 3];
            dst[i + 5] = src[i + 2];
            dst[i + 6] = src[i + 1];
            dst[i + 7] = src[i];
        }
    }
    /* copy len%8 remaining bytes */
    if (i < len) {
        uint8_t *dst = dst_;
        const uint8_t *src = src_;
        for (size_t j = 0; j < len-i; j++) {
            dst[i + j] = src[i+7 - j];
        }
    }
}

/*
 * Decode a big-endian length len vector of (unsigned char) into a length
 * len/8 vector of (uint64_t).  Assumes len is a multiple of 8.
 */
#define be64dec_vect be64enc_vect

#endif /* __BYTE_ORDER__ != __ORDER_BIG_ENDIAN__ */

/**
 * @brief    Elementary functions used by SHA512
 * @{
 */
#define Ch(x, y, z) ((x & (y ^ z)) ^ z)
#define Maj(x, y, z)    ((x & (y | z)) | (y & z))
#define SHR(x, n)   (x >> n)
#define ROTR(x, n)  ((x >> n) | (x << (64 - n)))
#define S0(x)       (ROTR(x, 28) ^ ROTR(x, 34) ^ ROTR(x, 39))
#define S1(x)       (ROTR(x, 14) ^ ROTR(x, 18) ^ ROTR(x, 41))
#define s0(x)       (ROTR(x, 1)  ^ ROTR(x, 8)  ^ SHR(x, 7))
#define s1(x)       (ROTR(x, 19) ^ ROTR(x, 61) ^ SHR(x, 6))
/** @} */

/** @brief SHA-384, SHA-512, SHA-512/224 and SHA-512/256 Constants */
static const uint64_t K[80] = {
    0x428a2f98d728ae22, 0x7137449123ef65cd, 0xb5c0fbcfec4d3b2f, 0xe9b5dba58189dbbc,
    0x3956c25bf348b538, 0x59f111f1b605d019, 0x923f82a4af194f9b, 0xab1c5ed5da6d8118,
    0xd807aa98a3030242, 0x12835b0145706fbe, 0x243185be4ee4b28c, 0x550c7dc3d5ffb4e2,
    0x72be5d74f27b896f, 0x80deb1fe3b1696b1, 0x9bdc06a725c71235, 0xc19bf174cf692694,
    0xe49b69c19ef14ad2, 0xefbe4786384f25e3, 0x0fc19dc68b8cd5b5, 0x240ca1cc77ac9c65,
    0x2de92c6f592b0275, 0x4a7484aa6ea6e483, 0x5cb0a9dcbd41fbd4, 0x76f988da831153b5,
    0x983e5152ee66dfab, 0xa831c66d2db43210, 0xb00327c898fb213f, 0xbf597fc7beef0ee4,
    0xc6e00bf33da88fc2, 0xd5a79147930aa725, 0x06ca6351e003826f, 0x142929670a0e6e70,
    0x27b70a8546d22ffc, 0x2e1b21385c26c926, 0x4d2c6dfc5ac42aed, 0x53380d139d95b3df,
    0x650a73548baf63de, 0x766a0abb3c77b2a8, 0x81c2c92e47edaee6, 0x92722c851482353b,
    0xa2bfe8a14cf10364, 0xa81a664bbc423001, 0xc24b8b70d0f89791, 0xc76c51a30654be30,
    0xd192e819d6ef5218, 0xd69906245565a910, 0xf40e35855771202a, 0x106aa07032bbd1b8,
    0x19a4c116b8d2d0c8, 0x1e376c085141ab53, 0x2748774cdf8eeb99, 0x34b0bcb5e19b48a8,
    0x391c0cb3c5c95a63, 0x4ed8aa4ae3418acb, 0x5b9cca4f7763e373, 0x682e6ff3d6b2b8a3,
    0x748f82ee5defb2fc, 0x78a5636f43172f60, 0x84c87814a1f0ab72, 0x8cc702081a6439ec,
    0x90befffa23631e28, 0xa4506cebde82bde9, 0xbef9a3f7b2c67915, 0xc67178f2e372532b,
    0xca273eceea26619c, 0xd186b8c721c0c207, 0xeada7dd6cde0eb1e, 0xf57d4f7fee6ed178,
    0x06f067aa72176fba, 0x0a637dc5a2c898a6, 0x113f9804bef90dae, 0x1b710b35131c471b,
    0x28db77f523047d84, 0x32caab7b40c72493, 0x3c9ebe0a15c9bebc, 0x431d67c49c100d4c,
    0x4cc5d4becb3e42b6, 0x597f299cfc657e2a, 0x5fcb6fab3ad6faec, 0x6c44198c4a475817,
};

/*
 * SHA512 block compression function.  The 512-bit state is transformed via
 * the 1024-bit input block to produce a new state.
 */
static void sha512_transform(uint64_t *state, const unsigned char block[128])
{
    uint64_t W[80];
    uint64_t S[8];

    /* 1. Prepare message schedule W. */
    be64dec_vect(W, block, 128);
    for (int i = 16; i < 80; i++) {
        W[i] = s1(W[i - 2]) + W[i - 7] + s0(W[i - 15]) + W[i - 16];
    }

    /* 2. Initialize working variables. */
    memcpy(S, state, 64);

    /* 3. Mix. */
    for (int i = 0; i < 80; ++i) {
        uint64_t e = S[(84 - i) % 8], f = S[(85 - i) % 8];
        uint64_t g = S[(86 - i) % 8], h = S[(87 - i) % 8];
        uint64_t t0 = h + S1(e) + Ch(e, f, g) + W[i] + K[i];

        uint64_t a = S[(80 - i) % 8], b = S[(81 - i) % 8];
        uint64_t c = S[(82 - i) % 8], d = S[(83 - i) % 8];
        uint64_t t1 = S0(a) + Maj(a, b, c);

        S[(83 - i) % 8] = d + t0;
        S[(87 - i) % 8] = t0 + t1;
    }

    /* 4. Mix local working variables into global state */
    for (int i = 0; i < 8; i++) {
        state[i] += S[i];
    }
}

static const unsigned char PAD[128] = {
    0x80, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
    0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0, 0,
};

/* Add padding and terminating bit-count. */
void sha512_common_pad(sha512_common_context_t *ctx)
{
    assert(ctx != NULL);

    /*
     * Convert length to a vector of bytes -- we do this now rather
     * than later because the length will change after we pad.
     */
    unsigned char len[16];

    be64enc_vect(len, ctx->count, 16);

    /* Add 1--128 bytes so that the resulting length is 112 mod 128 */
    uint8_t r = (ctx->count[1] >> 3) & 0x7f;
    uint8_t plen = (r < 112) ? (112 - r) : (240 - r);
    sha512_common_update(ctx, PAD, (size_t) plen);

    /* Add the terminating bit-count */
    sha512_common_update(ctx, len, 16);
}

/* Add bytes into the hash */
void sha512_common_update(sha512_common_context_t *ctx, const void *data, size_t len)
{
    assert(ctx != NULL);

    /* Number of bytes left in the buffer from previous updates */
    uint8_t r = (ctx->count[1] >> 3) & 0x7f;
    /* Number of bytes free in the buffer from previous updates */
    uint8_t f = 128 - r;

    /* Convert the length into a number of bits */
    uint64_t bitlen1 = ((uint64_t) len) << 3;
    uint64_t bitlen0 = ((uint64_t) len) >> 61;

    /* Update number of bits */
    if ((ctx->count[1] += bitlen1) < bitlen1) {
        ctx->count[0]++;
    }

    ctx->count[0] += bitlen0;

    /* Handle the case where we don't need to perform any transforms */
    if (len < f) {
        if (len > 0) {
            memcpy(&ctx->buf[r], data, len);
        }
        return;
    }

    /* Finish the current block */
    const unsigned char *src = data;

    memcpy(&ctx->buf[r], src, f);
    sha512_transform(ctx->state, ctx->buf);
    src += f;
    len -= f;

    /* Perform complete blocks */
    while (len >= 128) {
        sha512_transform(ctx->state, src);
        src += 128;
        len -= 128;
    }

    /* Copy left over data into buffer */
    memcpy(ctx->buf, src, len);
}

/*
 * SHA-512 finalization.  Pads the input data, exports the hash value,
 * and clears the context state.
 */
void sha512_common_final(sha512_common_context_t *ctx, void *dst, size_t dig_len)
{
    assert(ctx != NULL);

    /* Add padding */
    sha512_common_pad(ctx);

    /* Write the hash */
    be64enc_vect(dst, ctx->state, dig_len);

    /* Clear the context state */
    memset(ctx, 0, sizeof(*ctx));
}
