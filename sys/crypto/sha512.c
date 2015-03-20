/*
 * Copyright (C) 2015 Jan Wagner <mail@jwagner.eu>
 * Copyright (C) 2014 Daniel Beer <dlbeer@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_crypto
 * @{
 *
 * @file
 * @brief       Implementation of the SHA512 hash function
 *
 * @author      Jan Wagner <mail@jwagner.eu>
 * @author      Daniel Beer <dlbeer@gmail.com>
 * @author	NSA
 *
 */

#include "crypto/sha512.h"

/* Initial SHA-512 state */
const sha512_ctx sha512_initial_state = { {
        0x6a09e667f3bcc908LL, 0xbb67ae8584caa73bLL,
        0x3c6ef372fe94f82bLL, 0xa54ff53a5f1d36f1LL,
        0x510e527fade682d1LL, 0x9b05688c2b3e6c1fLL,
        0x1f83d9abfb41bd6bLL, 0x5be0cd19137e2179LL,
    }
};

/* SHA-512 internal round constants */
static const uint64_t round_k[80] = {
    0x428a2f98d728ae22LL, 0x7137449123ef65cdLL,
    0xb5c0fbcfec4d3b2fLL, 0xe9b5dba58189dbbcLL,
    0x3956c25bf348b538LL, 0x59f111f1b605d019LL,
    0x923f82a4af194f9bLL, 0xab1c5ed5da6d8118LL,
    0xd807aa98a3030242LL, 0x12835b0145706fbeLL,
    0x243185be4ee4b28cLL, 0x550c7dc3d5ffb4e2LL,
    0x72be5d74f27b896fLL, 0x80deb1fe3b1696b1LL,
    0x9bdc06a725c71235LL, 0xc19bf174cf692694LL,
    0xe49b69c19ef14ad2LL, 0xefbe4786384f25e3LL,
    0x0fc19dc68b8cd5b5LL, 0x240ca1cc77ac9c65LL,
    0x2de92c6f592b0275LL, 0x4a7484aa6ea6e483LL,
    0x5cb0a9dcbd41fbd4LL, 0x76f988da831153b5LL,
    0x983e5152ee66dfabLL, 0xa831c66d2db43210LL,
    0xb00327c898fb213fLL, 0xbf597fc7beef0ee4LL,
    0xc6e00bf33da88fc2LL, 0xd5a79147930aa725LL,
    0x06ca6351e003826fLL, 0x142929670a0e6e70LL,
    0x27b70a8546d22ffcLL, 0x2e1b21385c26c926LL,
    0x4d2c6dfc5ac42aedLL, 0x53380d139d95b3dfLL,
    0x650a73548baf63deLL, 0x766a0abb3c77b2a8LL,
    0x81c2c92e47edaee6LL, 0x92722c851482353bLL,
    0xa2bfe8a14cf10364LL, 0xa81a664bbc423001LL,
    0xc24b8b70d0f89791LL, 0xc76c51a30654be30LL,
    0xd192e819d6ef5218LL, 0xd69906245565a910LL,
    0xf40e35855771202aLL, 0x106aa07032bbd1b8LL,
    0x19a4c116b8d2d0c8LL, 0x1e376c085141ab53LL,
    0x2748774cdf8eeb99LL, 0x34b0bcb5e19b48a8LL,
    0x391c0cb3c5c95a63LL, 0x4ed8aa4ae3418acbLL,
    0x5b9cca4f7763e373LL, 0x682e6ff3d6b2b8a3LL,
    0x748f82ee5defb2fcLL, 0x78a5636f43172f60LL,
    0x84c87814a1f0ab72LL, 0x8cc702081a6439ecLL,
    0x90befffa23631e28LL, 0xa4506cebde82bde9LL,
    0xbef9a3f7b2c67915LL, 0xc67178f2e372532bLL,
    0xca273eceea26619cLL, 0xd186b8c721c0c207LL,
    0xeada7dd6cde0eb1eLL, 0xf57d4f7fee6ed178LL,
    0x06f067aa72176fbaLL, 0x0a637dc5a2c898a6LL,
    0x113f9804bef90daeLL, 0x1b710b35131c471bLL,
    0x28db77f523047d84LL, 0x32caab7b40c72493LL,
    0x3c9ebe0a15c9bebcLL, 0x431d67c49c100d4cLL,
    0x4cc5d4becb3e42b6LL, 0x597f299cfc657e2aLL,
    0x5fcb6fab3ad6faecLL, 0x6c44198c4a475817LL,
};

/* Inline load 8bit array into 64bit variable*/
static inline uint64_t load64(const uint8_t *x)
{
    uint64_t r;

    r = *(x++);
    r = (r << 8) | *(x++);
    r = (r << 8) | *(x++);
    r = (r << 8) | *(x++);
    r = (r << 8) | *(x++);
    r = (r << 8) | *(x++);
    r = (r << 8) | *(x++);
    r = (r << 8) | *(x++);

    return r;
}

/* Inline store 64bit variable into 8bit array */
static inline void store64(uint8_t *x, uint64_t v)
{
    x += 7;
    *(x--) = v;
    v >>= 8;
    *(x--) = v;
    v >>= 8;
    *(x--) = v;
    v >>= 8;
    *(x--) = v;
    v >>= 8;
    *(x--) = v;
    v >>= 8;
    *(x--) = v;
    v >>= 8;
    *(x--) = v;
    v >>= 8;
    *(x--) = v;
}

/* Inline 64bit rotation */
static inline uint64_t rot64(uint64_t x, int bits)
{
    return (x >> bits) | (x << (64 - bits));
}


/* Set up a new SHA-512 context */
void sha512_init(sha512_ctx *ctx)
{
    memcpy(ctx, &sha512_initial_state, sizeof(*ctx));
}

/* Add bytes to hash and perform SHA512 block compression */
void sha512_block(sha512_ctx *s, const uint8_t *block)
{
    uint64_t w[16];
    uint64_t a, b, c, d, e, f, g, h;
    int i;

    for (i = 0; i < 16; i++) {
        w[i] = load64(block);
        block += 8;
    }

    /* Load state */
    a = s->h[0];
    b = s->h[1];
    c = s->h[2];
    d = s->h[3];
    e = s->h[4];
    f = s->h[5];
    g = s->h[6];
    h = s->h[7];

    for (i = 0; i < 80; i++) {
        /* Compute value of w[i + 16]. w[wrap(i)] is currently w[i] */
        const uint64_t wi = w[i & 15];
        const uint64_t wi15 = w[(i + 1) & 15];
        const uint64_t wi2 = w[(i + 14) & 15];
        const uint64_t wi7 = w[(i + 9) & 15];
        const uint64_t s0 =
            rot64(wi15, 1) ^ rot64(wi15, 8) ^ (wi15 >> 7);
        const uint64_t s1 =
            rot64(wi2, 19) ^ rot64(wi2, 61) ^ (wi2 >> 6);

        /* Round calculations */
        const uint64_t S0 = rot64(a, 28) ^ rot64(a, 34) ^ rot64(a, 39);
        const uint64_t S1 = rot64(e, 14) ^ rot64(e, 18) ^ rot64(e, 41);
        const uint64_t ch = (e & f) ^ ((~e) & g);
        const uint64_t temp1 = h + S1 + ch + round_k[i] + wi;
        const uint64_t maj = (a & b) ^ (a & c) ^ (b & c);
        const uint64_t temp2 = S0 + maj;

        /* Update round state */
        h = g;
        g = f;
        f = e;
        e = d + temp1;
        d = c;
        c = b;
        b = a;
        a = temp1 + temp2;

        /* w[wrap(i)] becomes w[i + 16] */
        w[i & 15] = wi + s0 + wi7 + s1;
    }

    /* Store state */
    s->h[0] += a;
    s->h[1] += b;
    s->h[2] += c;
    s->h[3] += d;
    s->h[4] += e;
    s->h[5] += f;
    s->h[6] += g;
    s->h[7] += h;
}

/* SHA-512 finalization */
void sha512_final(sha512_ctx *s, const uint8_t *block, uint64_t total_size)
{
    uint8_t temp[SHA512_BLOCK_SIZE] = {0};
    const size_t last_size = total_size & (SHA512_BLOCK_SIZE - 1);

    if (last_size) {
        memcpy(temp, block, last_size);
    }

    temp[last_size] = 0x80;

    if (last_size > 111) {
        sha512_block(s, temp);
        memset(temp, 0, sizeof(temp));
    }

    /* Note: we assume total_size fits in 61 bits */
    store64(temp + SHA512_BLOCK_SIZE - 8, total_size << 3);

    sha512_block(s, temp);
}

/* Read out bytes from SHA-512 state */
void sha512_get(const sha512_ctx *s, uint8_t *hash, unsigned int offset, unsigned int len)
{
    int i;

    if (offset > SHA512_BLOCK_SIZE) {
        return;
    }

    if (len > SHA512_BLOCK_SIZE - offset) {
        len = SHA512_BLOCK_SIZE - offset;
    }

    /* Skip whole words */
    i = offset >> 3;
    offset &= 7;

    /* Skip/read out bytes */
    if (offset) {
        uint8_t tmp[8];
        unsigned int c = 8 - offset;

        if (c > len) {
            c = len;
        }

        store64(tmp, s->h[i++]);
        memcpy(hash, tmp + offset, c);
        len -= c;
        hash += c;
    }

    /* Read out whole words */
    while (len >= 8) {
        store64(hash, s->h[i++]);
        hash += 8;
        len -= 8;
    }

    /* Read out bytes */
    if (len) {
        uint8_t tmp[8];

        store64(tmp, s->h[i]);
        memcpy(hash, tmp, len);
    }
}
