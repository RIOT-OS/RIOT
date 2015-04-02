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
 * @brief       Headers for the implementation of the SkipJack cipher-algorithm
 *
 * @author      Freie Universitaet Berlin, Computer Systems & Telematics
 * @author      Nicolai Schmittberger <nicolai.schmittberger@fu-berlin.de>
 * @author      Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 */

#ifndef SKIPJACK_H_
#define SKIPJACK_H_

#include "crypto/ciphers.h"

#ifdef __cplusplus
extern "C" {
#endif

#define F(addr) /*CRYPTO_TABLE_ACCESS( &SJ_F[addr])*/ (SJ_F[addr])

// G-Permutation: 4 round feistel structure
#define G(key, b, bLeft, bRight)     \
     (                               \
       bLeft   = b,                  \
       bRight  = (b >> 8),           \
       bLeft  ^= F(bRight ^ key[0]), \
       bRight ^= F(bLeft  ^ key[1]), \
       bLeft  ^= F(bRight ^ key[2]), \
       bRight ^= F(bLeft  ^ key[3]), \
       ((bRight << 8) | bLeft))

#define G_INV(key, b, bLeft, bRight) \
     ( bLeft   = b,                  \
       bRight  = (b >> 8),           \
       bRight ^= F(bLeft  ^ key[3]), \
       bLeft  ^= F(bRight ^ key[2]), \
       bRight ^= F(bLeft  ^ key[1]), \
       bLeft  ^= F(bRight ^ key[0]), \
       ((bRight << 8) | bLeft))

// A-RULE:
#define RULE_A(skey, w1, w2, w3, w4, counter, tmp, bLeft, bRight ) { \
    tmp = w4;                                \
    w4 = w3;                                 \
    w3 = w2;                                 \
    w2 = G(skey, w1, bLeft, bRight);         \
    w1 = ((tmp ^ w2) ^ counter);             \
    counter++;                               \
    skey += 4; }

#define RULE_A_INV(skey, w1, w2, w3, w4, counter, tmp, bLeft, bRight) { \
    tmp = w4;                                \
    w4 = (w1 ^ w2 ^ counter);                \
    w1 = G_INV(skey, w2, bLeft, bRight);     \
    w2 = w3;                                 \
    w3 = tmp;                                \
    counter--;                               \
    skey -= 4; }                             \

// B-RULE:
#define RULE_B(skey, w1, w2, w3, w4, counter, tmp, bLeft, bRight ) { \
    tmp = w1;                                \
    w1 = w4;                                 \
    w4 = w3;                                 \
    w3 = (tmp ^ w2 ^ counter);               \
    w2 = G(skey, tmp, bLeft, bRight);        \
    counter++;                               \
    skey += 4; }

#define RULE_B_INV(skey, w1, w2, w3, w4, counter, tmp, bLeft, bRight ) { \
    tmp = w1;                                \
    w1 = G_INV(skey, w2, bLeft, bRight);     \
    w2 = (w1 ^ w3 ^ counter);                \
    w3 = w4;                                 \
    w4 = tmp;                                \
    counter--;                               \
    skey -= 4; }

/** 2 times keysize. makes unrolling keystream easier / efficient */
#define SKIPJACK_CONTEXT_SIZE 20

/**
 * @brief   Initialize the SkipJack-BlockCipher context.
 *
 * @param   context     structure to hold the opaque data from this
 *                      initialization call. It should be passed to future
 *                      invocations of this module which use this particular
 *                      key.
 * @param   keySize     key size in bytes
 * @param   key         pointer to the key
 *
 * @return  Whether initialization was successful. The command may be
 *          unsuccessful if the key size is not valid.
 */
int skipjack_init(cipher_context_t *context, const uint8_t *key, uint8_t keySize);

/**
 * @brief   Encrypts a single block (of blockSize) using the passed context.
 *
 * @param   context       holds the module specific opaque data related to the
 *                        key (perhaps key expansions).
 * @param   plainBlock    a plaintext block of blockSize
 * @param   cipherBlock   the resulting ciphertext block of blockSize
 *
 * @return  Whether the encryption was successful. Possible failure reasons
 *          include not calling init().
 */
int skipjack_encrypt(const cipher_context_t *context, const uint8_t *plainBlock,
                     uint8_t *cipherBlock);

/**
 * @brief   Decrypts a single block (of blockSize) using the passed context.
 *
 * @param   context       holds the module specific opaque data related to the
 *                        key (perhaps key expansions).
 * @param   cipherBlock   a ciphertext block of blockSize
 * @param   plainBlock    the resulting plaintext block of blockSize
 *
 * @return  Whether the decryption was successful. Possible failure reasons
 *         include not calling init()
 */
int skipjack_decrypt(const cipher_context_t *context, const uint8_t *cipherBlock,
                     uint8_t *plainBlock);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* SKIPJACK_H_ */
