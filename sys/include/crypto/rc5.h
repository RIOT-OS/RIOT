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
 * @brief       Headers for the implementation of the RC5 Cipher-Algorithm
 *
 * @author      Freie Universitaet Berlin, Computer Systems & Telematics
 * @author      Nicolai Schmittberger <nicolai.schmittberger@fu-berlin.de>
 * @author      Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 */

#include "crypto/ciphers.h"

#ifndef RC5_H_
#define RC5_H_

#ifdef __cplusplus
extern "C" {
#endif

#define RC5_32_P        0xB7E15163L
#define RC5_32_Q        0x9E3779B9L
#define RC5_32_MASK     0xffffffffL

#define RC5_ROUNDS 12

#define rotl32(a,b) fastrol32((a), (b))
#define rotr32(a,b) fastror32((a), (b))
#define rol32(a, n) ( a = (((a) << (n)) | ((a) >> (32-(n)))))
#define ror32(a, n) ( a = (((a) >> (n)) | ((a) << (32-(n)))))
#define fastrol32 rol32
#define fastror32 ror32

// convert a 4byte char array to a unsigned long
// [assumes least significant byte of char array is first]
#define c2l(c,l)  (l =((unsigned long)(*((c)))),        \
                   l|=((unsigned long)(*((c+1))))<< 8L, \
                   l|=((unsigned long)(*((c+2))))<<16L, \
                   l|=((unsigned long)(*((c+3))))<<24L)

// convert an unsigned long to a 4 byte char array
// [assumes least significant byte of char array is first]
#define l2c(l,c)  (*((c))  =(unsigned char)(((l)     )&0xff), \
                   *((c+1))=(unsigned char)(((l)>> 8L)&0xff), \
                   *((c+2))=(unsigned char)(((l)>>16L)&0xff), \
                   *((c+3))=(unsigned char)(((l)>>24L)&0xff))

// 2 * (ROUNDS +1) * 4
// 2 * 13 * 4 = 104 bytes
#define RC5_CONTEXT_SIZE (2 * (RC5_ROUNDS + 1))

/**
 * @brief the cipher_context_t adapted for RC5
 */
typedef struct {
    /** @cond INTERNAL */
    uint32_t skey [RC5_CONTEXT_SIZE];
    /** @endcond */
} rc5_context_t;

/**
* @brief    Initialize the BlockCipher.
*
* @param    context       the cipher_context_t-struct to save the initialization
*                         of the cipher in
* @param    keySize       the size of the key
* @param    key           a pointer to the key
*
* @return   Whether initialization was successful. The command may be
*           unsuccessful if the key size is not valid for the
*           given cipher implementation.
*/
int rc5_init(cipher_context_t *context, const uint8_t *key, uint8_t keySize);

/**
 * @brief   Encrypts a single block (of blockSize) using the passed context.
 *
 * PROLOGUE: 24 cycles
 * INIT:     48 cycles
 * LOOP:   1680 cycles (12 + fastrol [= 42] + 16) * 2 * RC5_ROUNDS
 * CLOSE:    24 cycles
 * =====================
 *         1776 cycles (avg case)
 *
 * @param   context       the cipher_context_t-struct to save the updated key in
 * @param   plain_block   a plaintext block of blockSize
 * @param   cipher_block  the resulting ciphertext block of blockSize
 *
 * @return  Whether the encryption was successful. Possible failure reasons
 *          include not calling init().
 */
int rc5_encrypt(const cipher_context_t *context, const uint8_t *plain_block, uint8_t *cipher_block);

/**
 * @brief   Decrypts a single block (of blockSize) using the key and the
 *          keySize.
 *
 * @param   context       the cipher_context_t-struct to use for this decryption
 * @param   cipherBlock   a ciphertext block of blockSize
 * @param   plainBlock    the resulting plaintext block of blockSize
 *
 * @return  Whether the decryption was successful. Possible failure reasons
 *          include not calling init() or an unimplimented decrypt function.
 */
int rc5_decrypt(const cipher_context_t *context, const uint8_t *cipherBlock,
                uint8_t *plainBlock);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* RC5_H_ */
