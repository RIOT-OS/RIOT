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
 * @brief       Headers for the packet encryption class. They are used to encrypt single packets.
 *
 * @author      Freie Universitaet Berlin, Computer Systems & Telematics
 * @author      Nicolai Schmittberger <nicolai.schmittberger@fu-berlin.de>
 * @author      Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 * @author      Mark Essien <markessien@gmail.com>
 */

#ifndef __CIPHERS_H_
#define __CIPHERS_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/* Shared header file for all cipher algorithms */

/* Set the algorithms that should be compiled in here. When these defines
 * are set, then packets will be compiled 5 times.
 */
// #define CRYPTO_THREEDES
// #define CRYPTO_AES
// #define CRYPTO_TWOFISH
// #define CRYPTO_SKIPJACK

/** @brief the length of keys in bytes */
#define CIPHERS_MAX_KEY_SIZE 20
#define CIPHER_MAX_BLOCK_SIZE 16


/**
 * Context sizes needed for the different ciphers.
 * Always order by number of bytes descending!!! <br><br>
 *
 * threedes     needs 24  bytes                           <br>
 * aes          needs CIPHERS_MAX_KEY_SIZE bytes          <br>
 * twofish      needs CIPHERS_MAX_KEY_SIZE bytes          <br>
 * skipjack     needs 20 bytes
 */
#if defined(CRYPTO_THREEDES)
    #define CIPHER_MAX_CONTEXT_SIZE 24
#elif defined(CRYPTO_AES)
    #define CIPHER_MAX_CONTEXT_SIZE CIPHERS_MAX_KEY_SIZE
#elif defined(CRYPTO_TWOFISH)
    #define CIPHER_MAX_CONTEXT_SIZE CIPHERS_MAX_KEY_SIZE
#elif defined(CRYPTO_SKIPJACK)
    #define CIPHER_MAX_CONTEXT_SIZE 20
#else
    // 0 is not a possibility because 0-sized arrays are not allowed in ISO C
    #define CIPHER_MAX_CONTEXT_SIZE 1
#endif

/**
 * error codes
 */
#define CIPHER_ERR_INVALID_KEY_SIZE   -3
#define CIPHER_ERR_INVALID_LENGTH     -4
#define CIPHER_ERR_ENC_FAILED         -5
#define CIPHER_ERR_DEC_FAILED         -6

/**
 * @brief   the context for cipher-operations
 */
typedef struct {
    uint8_t context[CIPHER_MAX_CONTEXT_SIZE];  /**< buffer for cipher operations */
} cipher_context_t;


/**
 * @brief   BlockCipher-Interface for the Cipher-Algorithms
 */
typedef struct cipher_interface_st {
    /** Blocksize of this cipher */
    uint8_t block_size;

    /** Maximum key size for this cipher */
    uint8_t max_key_size;

    /** the init function */
    int (*init)(cipher_context_t* ctx, const uint8_t* key, uint8_t key_size);

    /** the encrypt function */
    int (*encrypt)(const cipher_context_t* ctx, const uint8_t* plain_block,
                   uint8_t* cipher_block);

    /** the decrypt function */
    int (*decrypt)(const cipher_context_t* ctx, const uint8_t* cipher_block,
                   uint8_t* plain_block);
} cipher_interface_t;


typedef const cipher_interface_t *cipher_id_t;

extern const cipher_id_t CIPHER_3DES;
extern const cipher_id_t CIPHER_AES_128;
extern const cipher_id_t CIPHER_TWOFISH;
extern const cipher_id_t CIPHER_SKIPJACK;


/**
 * @brief basic struct for using block ciphers
 *        contains the cipher interface and the context
 */
typedef struct {
    const cipher_interface_t* interface; /**< BlockCipher-Interface for the
                                              Cipher-Algorithms */
    cipher_context_t context;            /**< The encryption context (buffer)
                                              for the algorithm */
} cipher_t;


/**
 * @brief Initialize new cipher state
 *
 * @param cipher     cipher struct to init (already allocated memory)
 * @param cipher_id  cipher algorithm id
 * @param key        encryption key to use
 * @param key_size   length of the encryption key
 */
int cipher_init(cipher_t* cipher, cipher_id_t cipher_id, const uint8_t* key,
                uint8_t key_size);


/**
 * @brief Encrypt data of BLOCK_SIZE length
 * *
 *
 * @param cipher     Already initialized cipher struct
 * @param input      pointer to input data to encrypt
 * @param output     pointer to allocated memory for encrypted data. It has to
 *                   be of size BLOCK_SIZE
 */
int cipher_encrypt(const cipher_t* cipher, const uint8_t* input, uint8_t* output);


/**
 * @brief Decrypt data of BLOCK_SIZE length
 * *
 *
 * @param cipher     Already initialized cipher struct
 * @param input      pointer to input data (of size BLOCKS_SIZE) to decrypt
 * @param output     pointer to allocated memory for decrypted data. It has to
 *                   be of size BLOCK_SIZE
 */
int cipher_decrypt(const cipher_t* cipher, const uint8_t* input, uint8_t* output);


/**
 * @brief Get block size of cipher
 * *
 *
 * @param cipher     Already initialized cipher struct
 */
int cipher_get_block_size(const cipher_t* cipher);


#ifdef __cplusplus
}
#endif

/** @} */
#endif /* __CIPHERS_H_ */
