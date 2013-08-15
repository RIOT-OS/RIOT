/*
 * RC5.h - The headers for the RC5 Cipher-Algorithm.
 *
 * Copyright (C) 2013 Freie Universität Berlin, Computer Systems & Telematics
 *
 * This source code is licensed under the LGPLv2 license,
 * See the file LICENSE for more details.
 */

/**
 * @file        RC5.h
 * @brief       Headers for the implementation of the RC5 Cipher-Algorithm
 *
 * @author      Freie Universitaet Berlin, Computer Systems & Telematics
 * @author      Nicolai Schmittberger <nicolai.schmittberger@fu-berlin.de>
 * @author      Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 *
 * @date        20.09.2013 17:26:21
 *
 */


#include "Ciphers.h"

#ifdef RC5

#ifndef RC5_H_
#define RC5_H_


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
/**
 * @brief the CipherContext adapted for RC5
 */
typedef struct RC5Context {
    uint32_t skey [2 * (RC5_ROUNDS + 1)];
} RC5Context;

enum { BSIZE = 8 };

/**
* @brief    Initialize the BlockCipher.
*
* @param    context       the CipherContext-struct to save the initialization
*                         of the cipher in
* @param    blockSize     the used blocksize - this must match the
*                         cipher-blocksize
* @param    keySize       the size of the key
* @param    key           a pointer to the key
*
* @return   Whether initialization was successful. The command may be
*           unsuccessful if the key size or blockSize are not valid for the
*           given cipher implementation.
*/
int RC5_init(CipherContext *context, uint8_t blockSize, uint8_t keySize,
             uint8_t *key);

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
 * @param   context       the CipherContext-struct to save the updated key in
 * @param   plainBlock    a plaintext block of blockSize
 * @param   cipherBlock   the resulting ciphertext block of blockSize
 *
 * @return  Whether the encryption was successful. Possible failure reasons
 *          include not calling init().
 */
int RC5_encrypt(CipherContext *context, uint8_t *block, uint8_t *cipherBlock);

/**
 * @brief   Decrypts a single block (of blockSize) using the key and the
 *          keySize.
 *
 * @param   context       the CipherContext-struct to use for this decryption
 * @param   cipherBlock   a ciphertext block of blockSize
 * @param   plainBlock    the resulting plaintext block of blockSize
 *
 * @return  Whether the decryption was successful. Possible failure reasons
 *          include not calling init() or an unimplimented decrypt function.
 */
int RC5_decrypt(CipherContext *context, uint8_t *cipherBlock,
                uint8_t *plainBlock);

/**
 * @brief   Sets up the key for usage with RC5
 *          Performs the key expansion on the real secret.
 *
 * @param   context       the CipherContext-struct to save the updated key in
 * @param   key           a pointer to the secret key
 * @param   keysize       the length of the secret key
 *
 * @return  SUCCESS
 */
int RC5_setup_key(CipherContext *context, uint8_t *key, uint8_t keysize);

/**
 * @brief   Returns the preferred block size that this cipher operates with.
 *          It is always safe to call this function before the init() call has
 *          been made.
 *
 * @return  the preferred block size for this cipher.
 */
uint8_t RC5_info_get_preferred_block_size(void);

/**
 * Interface to access the functions
 *
 */
block_cipher_interface_t RC5_interface = {
    "RC5\0",
    RC5_init,
    RC5_encrypt,
    RC5_decrypt,
    RC5_setup_key,
    RC5_info_get_preferred_block_size
};


#endif /* RC5_H_ */

#endif //RC5
