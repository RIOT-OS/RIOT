/*
 * SPDX-FileCopyrightText: 2013 Freie Universität Berlin, Computer Systems & Telematics
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     sys_crypto
 * @{
 *
 * @file
 * @brief       Headers for the implementation of the AES cipher-algorithm
 *
 * The default key size is 128 bits. To use a different key size add
 * USEMODULE += crypto_aes_192 and/or USEMODULE += crypto_aes_256 to
 * your Makefile.
 *
 * If only one key size is needed and that key size is not 128 bits, the 128 bit
 * `USEMODULE += crypto_aes_192` and/or `USEMODULE += crypto_aes_256` to
 * your Makefile.
 *
 * If only one key size is needed and that key size is not 128 bits, the 128 bit
 * key size can be disabled with `DISABLE_MODULE += crypto_aes_128` as an
 * optimization.
 *
 * @author      Nicolai Schmittberger <nicolai.schmittberger@fu-berlin.de>
 * @author      Fabrice Bellard
 * @author      Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 */

#include <stdint.h>

#include "compiler_hints.h"
#include "crypto/ciphers.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef uint32_t u32;
typedef uint16_t u16;
typedef uint8_t u8;

#define GETU32(pt)  (((u32)(pt)[0] << 24) ^ ((u32)(pt)[1] << 16) ^ \
                     ((u32)(pt)[2] <<  8) ^ ((u32)(pt)[3]))
#define PUTU32(ct, st)  { (ct)[0] = (u8)((st) >> 24); \
                          (ct)[1] = (u8)((st) >> 16); \
                          (ct)[2] = (u8)((st) >>  8); \
                          (ct)[3] = (u8)(st); }

#define AES_MAXNR         14
#define AES_BLOCK_SIZE    16

/**
 * @name AES key sizes
 * @{
 */
#define AES_KEY_SIZE_128    16
#define AES_KEY_SIZE_192    24
#define AES_KEY_SIZE_256    32
/** @} */

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
 * @param       context   the cipher_context_t-struct to save the
 *                        initialization of the cipher in
 * @param       keySize   the size of the key
 *                        Must be 16, since this implementation does not
 *                        support key lengths of 24 or 32 bytes
 * @param       key       a pointer to the key
 *
 * @retval  CIPHER_INIT_SUCCESS            the initialization was successful
 * @retval  CIPHER_ERR_BAD_CONTEXT_SIZE    `CIPHER_MAX_CONTEXT_SIZE` undefined
 *
 * @note    `CIPHER_ERR_BAD_CONTEXT_SIZE` error is typically when the
 *          cipher has not been included in the build
 */
ACCESS(read_only, 2, 3)
int aes_init(cipher_context_t *context, const uint8_t *key, uint8_t keySize);

/**
 * @brief   encrypts one plainBlock-block and saves the result in cipherblock.
 *          encrypts one blocksize long block of plaintext pointed to by
 *          plainBlock to one blocksize long block of ciphertext which will be
 *          written to the memory-area pointed to by cipherBlock
 *
 * @param       context       the cipher_context_t-struct to use for this
 *                            encryption
 * @param       plain_block   a pointer to the plaintext-block (of size
 *                            blocksize)
 * @param       cipher_block  a pointer to the place where the ciphertext will
 *                            be stored
 *
 * @retval      1             success
 * @retval      -1            @p context has not been initialized, see @p aes_init
 * @retval      -2            key size in @p context invalid (memory safety bug?)
 * @retval      <0            other error
 */
int aes_encrypt(const cipher_context_t *context, const uint8_t *plain_block,
                uint8_t *cipher_block);

/**
 * @brief   Decrypts one cipher-block and saves the plain-block in @p plain_block.
 *          Decrypts one blocksize long block of ciphertext pointed to by
 *          @p cipher_block to one blocksize long block of plaintext and stores
 *          the plaintext in the memory-area pointed to by @p plain_block.
 *
 * @param       context       the cipher_context_t-struct to use for this
 *                            decryption
 * @param       cipher_block  a pointer to the ciphertext-block (of size
 *                            blocksize) to be decrypted
 * @param       plain_block   a pointer to the place where the decrypted
 *                            plaintext will be stored
 *
 * @retval      1             success
 * @retval      -1            @p context has not been initialized, see @p aes_init
 * @retval      -2            key size in @p context invalid (memory safety bug?)
 * @retval      <0            other error
 */
int aes_decrypt(const cipher_context_t *context, const uint8_t *cipher_block,
                uint8_t *plain_block);

#ifdef __cplusplus
}
#endif

/** @} */
