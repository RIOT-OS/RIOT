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
 * @brief       Headers for the implementation of the AES cipher-algorithm
 *
 * @author      Freie Universitaet Berlin, Computer Systems & Telematics
 * @author      Nicolai Schmittberger <nicolai.schmittberger@fu-berlin.de>
 * @author      Fabrice Bellard
 * @author      Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 */

#ifndef AES_H
#define AES_H

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "crypto/ciphers.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;


/* This controls loop-unrolling in aes_core.c */
#undef FULL_UNROLL
# define GETU32(pt) (((u32)(pt)[0] << 24) ^ ((u32)(pt)[1] << 16) ^ \
                             ((u32)(pt)[2] <<  8) ^ ((u32)(pt)[3]))
# define PUTU32(ct, st) { (ct)[0] = (u8)((st) >> 24); \
                              (ct)[1] = (u8)((st) >> 16); \
                              (ct)[2] = (u8)((st) >>  8); \
                              (ct)[3] = (u8)(st); }

#define AES_MAXNR         14
#define AES_BLOCK_SIZE    16
#define AES_KEY_SIZE      16

/**
 * @brief AES key
 * @see cipher_context_t
 */
struct aes_key_st {
    /** @cond INTERNAL */
    uint32_t rd_key[4 * (AES_MAXNR + 1)];
    int rounds;
    /** @endcond */
};

typedef struct aes_key_st AES_KEY;

/**
 * @brief the cipher_context_t-struct adapted for AES
 */
typedef struct {
    /** context data buffer */
    uint32_t context[(4 * (AES_MAXNR + 1)) + 1];
} aes_context_t;

/**
 * @brief   initializes the AES Cipher-algorithm with the passed parameters
 *
 * @param       context   the cipher_context_t-struct to save the initialization
 *                        of the cipher in
 * @param       keySize   the size of the key
 * @param       key       a pointer to the key
 *
 * @return  Whether initialization was successful. The command may be
 *          unsuccessful if the key size is not valid.
 */
int aes_init(cipher_context_t *context, const uint8_t *key, uint8_t keySize);

/**
 * @brief   encrypts one plainBlock-block and saves the result in cipherblock.
 *          encrypts one blocksize long block of plaintext pointed to by
 *          plainBlock to one blocksize long block of ciphertext which will be
 *          written to the the memory-area pointed to by cipherBlock
 *
 * @param       context       the cipher_context_t-struct to use for this
 *                            encryption
 * @param       plain_block   a pointer to the plaintext-block (of size
 *                            blocksize)
 * @param       cipher_block  a pointer to the place where the ciphertext will
 *                            be stored
 *
 * @return  1 or result of aes_set_encrypt_key if it failed
 */
int aes_encrypt(const cipher_context_t *context, const uint8_t *plain_block,
                uint8_t *cipher_block);

/**
 * @brief   decrypts one cipher-block and saves the plain-block in plainBlock.
 *          decrypts one blocksize long block of ciphertext pointed to by
 *          cipherBlock to one blocksize long block of plaintext and stores
 *          the plaintext in the memory-area pointed to by plainBlock
 *
 * @param       context       the cipher_context_t-struct to use for this
 *                            decryption
 * @param       cipher_block  a pointer to the ciphertext-block (of size
 *                            blocksize) to be decrypted
 * @param       plain_block   a pointer to the place where the decrypted
 *                            plaintext will be stored
 *
 * @return  1 or negative value if cipher key cannot be expanded into
 *          decryption key schedule
 */
int aes_decrypt(const cipher_context_t *context, const uint8_t *cipher_block,
                uint8_t *plain_block);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* AES_H */
