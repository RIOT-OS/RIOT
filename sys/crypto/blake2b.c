/*
 * Copyright (C) 2015 Jan Wagner <mail@jwagner.eu>
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
 * @brief       Implementation of the BLAKE2b hash function
 *
 * @author      Jan Wagner <mail@jwagner.eu>
 * @author      Jean-Philippe Aumasson
 * @author      Samuel Neves
 * @author      Zooko Wilcox-O'Hearn
 * @author      Christian Winnerlein
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "crypto/blake2b.h"

/* BLAKE2b IVs */
static const uint64_t blake2b_IV[8] = {
    0x6a09e667f3bcc908ULL, 0xbb67ae8584caa73bULL,
    0x3c6ef372fe94f82bULL, 0xa54ff53a5f1d36f1ULL,
    0x510e527fade682d1ULL, 0x9b05688c2b3e6c1fULL,
    0x1f83d9abfb41bd6bULL, 0x5be0cd19137e2179ULL
};

/* BLAKE2b sigma constants */
static const uint8_t blake2b_sigma[12][16] = {
    {  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15 } ,
    { 14, 10,  4,  8,  9, 15, 13,  6,  1, 12,  0,  2, 11,  7,  5,  3 } ,
    { 11,  8, 12,  0,  5,  2, 15, 13, 10, 14,  3,  6,  7,  1,  9,  4 } ,
    {  7,  9,  3,  1, 13, 12, 11, 14,  2,  6,  5, 10,  4,  0, 15,  8 } ,
    {  9,  0,  5,  7,  2,  4, 10, 15, 14,  1, 11, 12,  6,  8,  3, 13 } ,
    {  2, 12,  6, 10,  0, 11,  8,  3,  4, 13,  7,  5, 15, 14,  1,  9 } ,
    { 12,  5,  1, 15, 14, 13,  4, 10,  0,  7,  6,  3,  9,  2,  8, 11 } ,
    { 13, 11,  7, 14, 12,  1,  3,  9,  5,  0, 15,  4,  8,  6,  2, 10 } ,
    {  6, 15, 14,  9, 11,  3,  0,  8, 12,  2, 13,  7,  1,  4, 10,  5 } ,
    { 10,  2,  8,  4,  7,  6,  1,  5, 15, 11,  9, 14,  3, 12, 13 , 0 } ,
    {  0,  1,  2,  3,  4,  5,  6,  7,  8,  9, 10, 11, 12, 13, 14, 15 } ,
    { 14, 10,  4,  8,  9, 15, 13,  6,  1, 12,  0,  2, 11,  7,  5,  3 }
};

/* Load 64 bit value */
static inline uint64_t load64(const void *src)
{
    return *(uint64_t *)(src);
}

/* Store 32 bit value into dst */
static inline void store32(void *dst, uint32_t w)
{
    *(uint32_t *)(dst) = w;
}

/* Store 64 bit value into dst */
static inline void store64(void *dst, uint64_t w)
{
    *(uint64_t *)(dst) = w;
}

/* Rotate right - 64 bit */
static inline uint64_t rotr64(const uint64_t w, const unsigned c)
{
    return (w >> c) | (w << (64 - c));
}

/* Prevents compiler optimizing out memset() */
static inline void secure_zero_memory(void *v, size_t n)
{
    volatile uint8_t *p = (volatile uint8_t *)v;

    while (n--) {
        *p++ = 0;
    }
}

/* Set BLAKE2b state last node flag */
static inline int blake2b_set_lastnode(blake2b_state *S)
{
    S->f[1] = ~0ULL;
    return 0;
}

/* Clear BLAKE2b state last node flag */
static inline int blake2b_clear_lastnode(blake2b_state *S)
{
    S->f[1] = 0ULL;
    return 0;
}

/* Some helper functions, not necessarily useful */
static inline int blake2b_set_lastblock(blake2b_state *S)
{
    if (S->last_node) {
        blake2b_set_lastnode(S);
    }

    S->f[0] = ~0ULL;
    return 0;
}

/* Clear BLAKE2b state last block flag */
static inline int blake2b_clear_lastblock(blake2b_state *S)
{
    if (S->last_node) {
        blake2b_clear_lastnode(S);
    }

    S->f[0] = 0ULL;
    return 0;
}

/* Increment BLAKE2b state counter */
static inline int blake2b_increment_counter(blake2b_state *S, const uint64_t inc)
{
    S->t[0] += inc;
    S->t[1] += (S->t[0] < inc);
    return 0;
}

/* Set digest length parameter value */
static inline int blake2b_param_set_digest_length(blake2b_param *P, const uint8_t digest_length)
{
    P->digest_length = digest_length;
    return 0;
}

/* Set fanout parameter value */
static inline int blake2b_param_set_fanout(blake2b_param *P, const uint8_t fanout)
{
    P->fanout = fanout;
    return 0;
}

/* Set max depth parameter value */
static inline int blake2b_param_set_max_depth(blake2b_param *P, const uint8_t depth)
{
    P->depth = depth;
    return 0;
}

/* Set leaf length parameter value */
static inline int blake2b_param_set_leaf_length(blake2b_param *P, const uint32_t leaf_length)
{
    store32(&P->leaf_length, leaf_length);
    return 0;
}

/* Set node offset parameter value */
static inline int blake2b_param_set_node_offset(blake2b_param *P, const uint64_t node_offset)
{
    store64(&P->node_offset, node_offset);
    return 0;
}

/* Set node depth parameter value */
static inline int blake2b_param_set_node_depth(blake2b_param *P, const uint8_t node_depth)
{
    P->node_depth = node_depth;
    return 0;
}

/* Set inner length parameter value */
static inline int blake2b_param_set_inner_length(blake2b_param *P, const uint8_t inner_length)
{
    P->inner_length = inner_length;
    return 0;
}

/* Set salt parameter value */
static inline int blake2b_param_set_salt(blake2b_param *P, const uint8_t salt[BLAKE2B_SALTBYTES])
{
    memcpy(P->salt, salt, BLAKE2B_SALTBYTES);
    return 0;
}

/* Set personal parameter value */
static inline int blake2b_param_set_personal(blake2b_param *P,
        const uint8_t personal[BLAKE2B_PERSONALBYTES])
{
    memcpy(P->personal, personal, BLAKE2B_PERSONALBYTES);
    return 0;
}

/* Initialize BLAKE2b state with IV constants */
static inline int blake2b_init0(blake2b_state *S)
{
    memset(S, 0, sizeof(blake2b_state));

    for (int i = 0; i < 8; ++i) {
        S->h[i] = blake2b_IV[i];
    }

    return 0;
}

/* Initialize xors IV with input parameter block */
int blake2b_init_param(blake2b_state *S, const blake2b_param *P)
{
    blake2b_init0(S);
    uint8_t *p = (uint8_t *)(P);

    /* IV XOR ParamBlock */
    for (size_t i = 0; i < 8; ++i) {
        S->h[i] ^= load64(p + sizeof(S->h[i]) * i);
    }

    return 0;
}

/* Sequential BLAKE2b initialization */
int blake2b_init(blake2b_state *S, const uint8_t outlen)
{
    blake2b_param P[1];

    if ((!outlen) || (outlen > BLAKE2B_OUTBYTES)) {
        return -1;
    }

    P->digest_length = outlen;
    P->key_length    = 0;
    P->fanout        = 1;
    P->depth         = 1;
    store32(&P->leaf_length, 0);
    store64(&P->node_offset, 0);
    P->node_depth    = 0;
    P->inner_length  = 0;
    memset(P->reserved, 0, sizeof(P->reserved));
    memset(P->salt,     0, sizeof(P->salt));
    memset(P->personal, 0, sizeof(P->personal));
    return blake2b_init_param(S, P);
}

/* Initialize BLAKE2b state with key */
int blake2b_init_key(blake2b_state *S, const uint8_t outlen, const void *key, const uint8_t keylen)
{
    blake2b_param P[1];

    if ((!outlen) || (outlen > BLAKE2B_OUTBYTES)) {
        return -1;
    }

    if (!key || !keylen || keylen > BLAKE2B_KEYBYTES) {
        return -1;
    }

    P->digest_length = outlen;
    P->key_length    = keylen;
    P->fanout        = 1;
    P->depth         = 1;
    store32(&P->leaf_length, 0);
    store64(&P->node_offset, 0);
    P->node_depth    = 0;
    P->inner_length  = 0;
    memset(P->reserved, 0, sizeof(P->reserved));
    memset(P->salt,     0, sizeof(P->salt));
    memset(P->personal, 0, sizeof(P->personal));

    if (blake2b_init_param(S, P) < 0) {
        return -1;
    }

    {
        uint8_t block[BLAKE2B_BLOCKBYTES];
        memset(block, 0, BLAKE2B_BLOCKBYTES);
        memcpy(block, key, keylen);
        blake2b_update(S, block, BLAKE2B_BLOCKBYTES);
        /* Burn the key from stack */
        secure_zero_memory(block, BLAKE2B_BLOCKBYTES);
    }

    return 0;
}

/* Perform BLAKE2b block compression */
static int blake2b_compress(blake2b_state *S, const uint8_t block[BLAKE2B_BLOCKBYTES])
{
    uint64_t m[16];
    uint64_t v[16];
    int i;

    for (i = 0; i < 16; ++i) {
        m[i] = load64(block + i * sizeof(m[i]));
    }

    for (i = 0; i < 8; ++i) {
        v[i] = S->h[i];
    }

    v[ 8] = blake2b_IV[0];
    v[ 9] = blake2b_IV[1];
    v[10] = blake2b_IV[2];
    v[11] = blake2b_IV[3];
    v[12] = S->t[0] ^ blake2b_IV[4];
    v[13] = S->t[1] ^ blake2b_IV[5];
    v[14] = S->f[0] ^ blake2b_IV[6];
    v[15] = S->f[1] ^ blake2b_IV[7];

    BLAKE2B_ROUND(0);
    BLAKE2B_ROUND(1);
    BLAKE2B_ROUND(2);
    BLAKE2B_ROUND(3);
    BLAKE2B_ROUND(4);
    BLAKE2B_ROUND(5);
    BLAKE2B_ROUND(6);
    BLAKE2B_ROUND(7);
    BLAKE2B_ROUND(8);
    BLAKE2B_ROUND(9);
    BLAKE2B_ROUND(10);
    BLAKE2B_ROUND(11);

    for (i = 0; i < 8; ++i) {
        S->h[i] = S->h[i] ^ v[i] ^ v[i + 8];
    }

    return 0;
}

/* Add bytes by performing BLAKE2b block compression */
int blake2b_update(blake2b_state *S, const uint8_t *in, uint64_t inlen)
{
    while (inlen > 0) {
        size_t left = S->buflen;
        size_t fill = 2 * BLAKE2B_BLOCKBYTES - left;

        if (inlen > fill) {
            /* Fill buffer */
            memcpy(S->buf + left, in, fill);
            S->buflen += fill;
            blake2b_increment_counter(S, BLAKE2B_BLOCKBYTES);

            /* Compress buffer */
            blake2b_compress(S, S->buf);

            /* Shift buffer left */
            memcpy(S->buf, S->buf + BLAKE2B_BLOCKBYTES, BLAKE2B_BLOCKBYTES);
            S->buflen -= BLAKE2B_BLOCKBYTES;
            in += fill;
            inlen -= fill;
        }
        else {
            /* inlen <= fill */
            memcpy(S->buf + left, in, inlen);

            /* Be lazy, do not compress */
            S->buflen += inlen;
            in += inlen;
            inlen -= inlen;
        }
    }

    return 0;
}

/* Finalize BLAKE2b hash output */
int blake2b_final(blake2b_state *S, uint8_t *out, uint8_t outlen)
{
    uint8_t buffer[BLAKE2B_OUTBYTES];

    if (S->buflen > BLAKE2B_BLOCKBYTES) {
        blake2b_increment_counter(S, BLAKE2B_BLOCKBYTES);
        blake2b_compress(S, S->buf);
        S->buflen -= BLAKE2B_BLOCKBYTES;
        memcpy(S->buf, S->buf + BLAKE2B_BLOCKBYTES, S->buflen);
    }

    blake2b_increment_counter(S, S->buflen);
    blake2b_set_lastblock(S);

    /* Padding */
    memset(S->buf + S->buflen, 0, 2 * BLAKE2B_BLOCKBYTES - S->buflen);
    blake2b_compress(S, S->buf);

    /* Output full hash to temp buffer */
    for (int i = 0; i < 8; ++i) {
        store64(buffer + sizeof(S->h[i]) * i, S->h[i]);
    }

    memcpy(out, buffer, outlen);
    return 0;
}

/* BLAKE2b hash function */
int blake2b(uint8_t *out, const void *in, const void *key, const uint8_t outlen,
            const uint64_t inlen, uint8_t keylen)
{
    blake2b_state S[1];

    /* Verify parameters */
    if (NULL == in) {
        return -1;
    }

    if (NULL == out) {
        return -1;
    }

    if (NULL == key) {
        keylen = 0;
    }

    if (keylen > 0) {
        if (blake2b_init_key(S, outlen, key, keylen) < 0) {
            return -1;
        }
    }
    else {
        if (blake2b_init(S, outlen) < 0) {
            return -1;
        }
    }

    blake2b_update(S, (uint8_t *)in, inlen);
    blake2b_final(S, out, outlen);
    return 0;
}
/** @} */
