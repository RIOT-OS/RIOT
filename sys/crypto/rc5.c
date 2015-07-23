/*
 * Copyright (C) 2013 Freie Universität Berlin, Computer Systems & Telematics
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
 * @brief       implementation of the RC5 cipher-algorithm
 *
 * @author      Nicolai Schmittberger <nicolai.schmittberger@fu-berlin.de>
 * @author      Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 * @author      Naveen Sastry
 *
 * @}
 */

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "crypto/rc5.h"
#include "crypto/ciphers.h"

/**
 * Define a fixed blocksize of 8 bytes
 */
#define BLOCK_SIZE      (8U)

/**
 * @brief Interface to the rc5 cipher
 */
static const cipher_interface_t rc5_interface = {
    BLOCK_SIZE,
    CIPHERS_MAX_KEY_SIZE,
    rc5_init,
    rc5_encrypt,
    rc5_decrypt
};
const cipher_id_t CIPHER_RC5 = &rc5_interface;


int rc5_encrypt(const cipher_context_t *context, const uint8_t *block,
                uint8_t *cipherBlock)
{
    register uint32_t l;
    register uint32_t r;
    rc5_context_t *rc5_context = (rc5_context_t *) context->context;
    register uint32_t *s = rc5_context->skey;
    uint8_t i;
    c2l(block, l);
    block += 4;
    c2l(block, r);
    l += *s++;
    r += *s++;

    for (i = RC5_ROUNDS; i > 0; i--) {
        l ^= r;
        uint8_t tmp = r;
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

int rc5_decrypt(const cipher_context_t *context, const uint8_t *cipherBlock,
                uint8_t *plainBlock)
{
    register uint32_t l;
    register uint32_t r;
    rc5_context_t *rc5_context = (rc5_context_t *) context->context;
    register uint32_t *s = rc5_context->skey + (2 * RC5_ROUNDS) + 1;
    uint8_t i;

    c2l(cipherBlock, l);
    cipherBlock += 4;
    c2l(cipherBlock, r);

    for (i = RC5_ROUNDS; i > 0; i--) {
        r -= *s--;
        uint8_t tmp = l;
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

int rc5_init(cipher_context_t *context, const uint8_t *key, uint8_t keySize)
{
    (void) keySize;
    uint32_t *L, l, A, B, *S;
    uint8_t ii, jj;
    int8_t i;
    uint8_t tmp[8];

    // Make sure that context is large enough. If this is not the case,
    // you should build with -DRC5.
    if(CIPHER_MAX_CONTEXT_SIZE < RC5_CONTEXT_SIZE) {
        return 0;
    }

    rc5_context_t *rc5_context = (rc5_context_t *) context->context;
    S = rc5_context->skey;

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
    S = rc5_context->skey;

    for (i = 3 * (2 * RC5_ROUNDS + 2) - 1; i >= 0; i--) {
        uint32_t k = (*S + A + B)&RC5_32_MASK;
        rotl32((k), (3));
        A = *S = k;
        S++;
        uint8_t m = ((char)(A + B)) & 0x1f;
        k = (*L + A + B)&RC5_32_MASK;
        rotl32((k), (m));
        B = *L = k;

        if (++ii >= 2 * RC5_ROUNDS + 2) {
            ii = 0;
            S = rc5_context->skey;
        }

        jj ^= 4;
        L = (uint32_t *)(&tmp[jj]);
    }

    return 1;
}
