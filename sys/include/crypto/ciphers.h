/*
 * Copyright (C) 2013 Freie Universität Berlin, Computer Systems & Telematics
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     sys_crypto
 * @{
 *
 * @file
 * @brief       Headers for the packet encryption class. They are used to encrypt single packets.
 *
 * @author      Freie Universitaet Berlin, Computer Systems & Telematics
 * @author      Nicolai Schmittberger <nicolai.schmittberger@fu-berlin.de>
 * @author      Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 * @author      Mark Essien <markessien@gmail.com>
 */

#include <stddef.h>
#include <stdint.h>

#include "modules.h"

#ifdef __cplusplus
extern "C" {
#endif

/* Shared header file for all cipher algorithms */

/** @brief the length of keys in bytes
 *
 * As of now AES is the only cipher which supports different key sizes.
 * Here we optimize the CIPHERS_MAX_KEY_SIZE to always have the smallest possible
 * value based on which AES key sizes are used.
 */
#if IS_USED(MODULE_CRYPTO_AES_256)
#  define CIPHERS_MAX_KEY_SIZE 32
#elif IS_USED(MODULE_CRYPTO_AES_192)
#  define CIPHERS_MAX_KEY_SIZE 24
#else
#  define CIPHERS_MAX_KEY_SIZE 16
#endif
#define CIPHER_MAX_BLOCK_SIZE 16

/**
 * Context sizes needed for the different ciphers.
 * Always order by number of bytes descending!!! <br><br>
 *
 * aes          needs CIPHERS_MAX_KEY_SIZE bytes          <br>
 */
#if IS_USED(MODULE_CRYPTO_AES_256) || IS_USED(MODULE_CRYPTO_AES_192) || \
    IS_USED(MODULE_CRYPTO_AES_128)
#  define CIPHER_MAX_CONTEXT_SIZE CIPHERS_MAX_KEY_SIZE
#else
/* 0 is not a possibility because 0-sized arrays are not allowed in ISO C */
#  define CIPHER_MAX_CONTEXT_SIZE 1
#endif

/* return codes */

#define CIPHER_ERR_INVALID_KEY_SIZE   (-3)
#define CIPHER_ERR_INVALID_LENGTH     (-4)
#define CIPHER_ERR_ENC_FAILED         (-5)
#define CIPHER_ERR_DEC_FAILED         (-6)
/** Is returned by the cipher_init functions, if the corresponding algorithm
 * has not been included in the build */
#define CIPHER_ERR_BAD_CONTEXT_SIZE    0
/**  Returned by cipher_init upon successful initialization of a cipher. */
#define CIPHER_INIT_SUCCESS            1

/**
 * @brief   the context for cipher-operations
 */
typedef struct {
    uint8_t key_size;                         /**< key size used */
    uint8_t context[CIPHER_MAX_CONTEXT_SIZE]; /**< buffer for cipher operations */
} cipher_context_t;

/**
 * @brief   BlockCipher-Interface for the Cipher-Algorithms
 */
typedef struct cipher_interface_st {
    /** @brief Blocksize of this cipher */
    uint8_t block_size;

    /**
     * @brief the init function.
     *
     * This function is responsible for checking that the given key_size is
     * valid for the chosen cipher.
     */
    int (*init)(cipher_context_t *ctx, const uint8_t *key, uint8_t key_size);

    /** @brief the encrypt function */
    int (*encrypt)(const cipher_context_t *ctx, const uint8_t *plain_block,
                   uint8_t *cipher_block);

    /** @brief the decrypt function */
    int (*decrypt)(const cipher_context_t *ctx, const uint8_t *cipher_block,
                   uint8_t *plain_block);
} cipher_interface_t;

/** Pointer type to BlockCipher-Interface for the Cipher-Algorithms */
typedef const cipher_interface_t *cipher_id_t;

/**
 * @brief AES cipher id
 */
extern const cipher_id_t CIPHER_AES;

/**
 * @brief basic struct for using block ciphers
 *        contains the cipher interface and the context
 */
typedef struct {
    const cipher_interface_t *interface; /**< BlockCipher-Interface for the
                                                 Cipher-Algorithms */
    cipher_context_t context;            /**< The encryption context (buffer)
                                                 for the algorithm */
} cipher_t;

/**
 * @brief Initialize new cipher state
 *
 * @param[out]  cipher      cipher struct to init (already allocated memory)
 * @param[in]   cipher_id   cipher algorithm id
 * @param[in]   key         encryption key to use
 * @param[in]   key_size    length of the encryption key
 *
 * @retval      CIPHER_INIT_SUCCESS         initialization successful
 * @retval      CIPHER_ERR_BAD_CONTEXT_SIZE `CIPHER_MAX_CONTEXT_SIZE` has not
 *                                          been defined
 * @retval      CIPHER_ERR_INVALID_KEY_SIZE @p key_size is not valid
 *
 * @note    The error `CIPHER_ERR_BAD_CONTEXT_SIZE` will occur when the `cipher`
 *          module is not enabled. Add `USEMODULE += cipher` to fix.
 */
int cipher_init(cipher_t *cipher, cipher_id_t cipher_id, const uint8_t *key,
                uint8_t key_size);

/**
 * @brief   Encrypt data of BLOCK_SIZE length
 * *
 *
 * @param[in]   cipher  Already initialized cipher struct
 * @param[in]   input   pointer to input data to encrypt
 * @param[out]  output  pointer to allocated memory for encrypted data. It has
 *                      to be of size `BLOCK_SIZE`
 *
 * @retval      1       Success
 * @retval      <0      Error
 */
int cipher_encrypt(const cipher_t *cipher, const uint8_t *input,
                   uint8_t *output);

/**
 * @brief   Decrypt data of BLOCK_SIZE length
 * *
 *
 * @param[in]   cipher  Already initialized cipher struct
 * @param[in]   input   pointer to input data (of size BLOCKS_SIZE) to decrypt
 * @param[out]  output  pointer to allocated memory for decrypted data. It
 *                      has to be of size BLOCK_SIZE
 *
 * @retval      1       Success
 * @retval      <0      Error
 */
int cipher_decrypt(const cipher_t *cipher, const uint8_t *input,
                   uint8_t *output);

/**
 * @brief   Get block size of cipher
 * *
 *
 * @param[in]   cipher  Already initialized cipher struct
 *
 * @return              The cipher's block size (in bytes)
 */
size_t cipher_get_block_size(const cipher_t *cipher);

#ifdef __cplusplus
}
#endif

/** @} */
