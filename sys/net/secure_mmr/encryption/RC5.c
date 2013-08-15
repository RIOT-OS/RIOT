/*
 * RC5.c - The RC5 Cipher-Algorithm.
 *
 * Copyright (C) 2013 Freie Universität Berlin, Computer Systems & Telematics
 *
 * This source code is licensed under the LGPLv2 license,
 * See the file LICENSE for more details.
 */

/**
 * @file        RC5.c
 * @brief       implementation of the RC5 cipher-algorithm
 *
 * @author      Freie Universität Berlin, Computer Systems & Telematics
 * @author      Nicolai Schmittberger <nicolai.schmittberger@fu-berlin.de>
 * @author      Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 * @author      Naveen Sastry
 *
 * @date        10.09.2013 12:57:21
 *
 */

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "RC5.h"

#ifdef RC5


int RC5_init(CipherContext *context, uint8_t blockSize, uint8_t keySize,
             uint8_t *key)
{
    // 8 byte blocks only
    if (blockSize != BSIZE) {
        return 0;
    }

    return RC5_setup_key(context, key, 0);
}


int RC5_encrypt(CipherContext *context, uint8_t *block,
                uint8_t *cipherBlock)
{
    register uint32_t l;
    register uint32_t r;
    register uint32_t *s = ((RC5Context *) context->context)->skey;
    uint8_t i, tmp;
    c2l(block, l);
    block += 4;
    c2l(block, r);
    l += *s++;
    r += *s++;

    for (i = RC5_ROUNDS; i > 0; i--) {
        l ^= r;
        tmp = r;
        tmp &= 0x1f;
        rotl32(l, tmp);
        l += *s++;
        r ^= l;
        tmp = l;
        tmp &= 0x1f;
        rotl32(r, tmp);
        r += *s++;
    }

    l2c(l, cipherBlock);
    cipherBlock += 4;
    l2c(r, cipherBlock);
    return 1;
}

int RC5_decrypt(CipherContext *context, uint8_t *cipherBlock,
                uint8_t *plainBlock)
{
    register uint32_t l;
    register uint32_t r;
    register uint32_t *s = ((RC5Context *) context->context)->skey +
                           (2 * RC5_ROUNDS) + 1;
    uint8_t i, tmp;

    c2l(cipherBlock, l);
    cipherBlock += 4;
    c2l(cipherBlock, r);

    for (i = RC5_ROUNDS; i > 0; i--) {
        r -= *s--;
        tmp = l;
        tmp &= 0x1f;
        rotr32(r, tmp);
        r ^= l;
        l -= *s--;
        tmp = r;
        tmp &= 0x1f;
        rotr32(l, tmp);
        l ^= r;
    }

    r -= *s--;
    l -= *s;
    l2c(l, plainBlock);
    plainBlock += 4;
    l2c(r, plainBlock);
    return 1;
}

int RC5_setup_key(CipherContext *context, uint8_t *key, uint8_t keysize)
{
    uint32_t *L, l, A, B, *S, k;
    uint8_t ii, jj, m;
    int8_t i;
    uint8_t tmp[8];
    S = ((RC5Context *)context->context)->skey;

    //dumpBuffer ("RC5M:setupKey K", (uint8_t *)key, 8);
    c2l(key, l);
    L = (uint32_t *) tmp;
    L[0] = l;
    key += 4;
    c2l(key, l);
    L[1] = l;
    S[0] = RC5_32_P;

    //dumpBuffer ("RC5M:setupKey L", (uint8_t *)L, 8);
    for (i = 1; i < 2 * RC5_ROUNDS + 2; i++) {
        S[i] = (S[i - 1] + RC5_32_Q);
        /* sum =(*S+RC5_32_Q)&RC5_32_MASK;
         * S++;
         * S = sum;
         */
    }

    //dumpBuffer ("RC5M: setupKey S", (uint8_t *)S, 2 * (RC5_ROUNDS +1) * 4);
    ii = jj = 0;
    A = B = 0;
    S = ((RC5Context *)context->context)->skey;

    for (i = 3 * (2 * RC5_ROUNDS + 2) - 1; i >= 0; i--) {
        k = (*S + A + B)&RC5_32_MASK;
        rotl32((k), (3));
        A = *S = k;
        S++;
        m = ((char)(A + B)) & 0x1f;
        k = (*L + A + B)&RC5_32_MASK;
        rotl32((k), (m));
        B = *L = k;

        if (++ii >= 2 * RC5_ROUNDS + 2) {
            ii = 0;
            S = ((RC5Context *)context->context)->skey;
        }

        jj ^= 4;
        L = (uint32_t *)(&tmp[jj]);
    }

    return 1;
}

/**
 * Returns the preferred block size that this cipher operates with. It is
 * always safe to call this function before the init() call has been made.
 *
 * @return the preferred block size for this cipher. In the case where the
 *         cipher operates with multiple block sizes, this will pick one
 *         particular size (deterministically).
 */
uint8_t RC5_info_get_preferred_block_size()
{
    return BSIZE;
}

#endif //RC5
