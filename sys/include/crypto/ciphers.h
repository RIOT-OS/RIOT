/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This source code is licensed under the LGPLv2 license,
 * See the file LICENSE for more details.
 */

/**
 * @ingroup     sys_crypto
 * @{
 *
 * @file        ciphers.h
 * @brief       generic API for block ciphers
 *
 * @author      Freie Universitaet Berlin, Computer Systems & Telematics
 * @author      Nicolai Schmittberger <nicolai.schmittberger@fu-berlin.de>
 * @author      Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 * @author      Mark Essien <markessien@gmail.com>
 * @author      Nico von Geyso <nico.geyso@fu-berlin.de>
 */

#ifndef __CRYPTO_CIPHERS_H_
#define __CRYPTO_CIPHERS_H_

#include "kernel.h"

#define CIPHERS_MAX_KEY_SIZE 20
#define CIPHER_MAX_BLOCK_SIZE 16

/**
 * error codes
 */
#define CIPHER_ERR_UNSUPPORTED_CIHPER -2
#define CIPHER_ERR_INVALID_KEY_SIZE   -3
#define CIPHER_ERR_INVALID_LENGTH     -4
#define CIPHER_ERR_ENC_FAILED         -5
#define CIPHER_ERR_DEC_FAILED         -6

/**
 * @brief   the context for cipher-operations
 *          always order by number of bytes descending!!! <br>
 * rc5          needs 104 bytes                           <br>
 * tripledes    needs 24  bytes                           <br>
 * aes          needs CIPHERS_MAX_KEY_SIZE bytes          <br>
 * twofish      needs CIPHERS_MAX_KEY_SIZE bytes          <br>
 * skipjack     needs 20 bytes                            <br>
 * identity     needs 1  byte                             <br>
 */
typedef struct {
#if defined(MODULE_RC5)
	uint8_t context[104];                  /* supports RC5 and lower       */
#elif defined(MODULE_3DES)
	uint8_t context[24];                   /* supports TripleDES and lower */
#elif defined(MODULE_AES)
	uint8_t context[CIPHERS_MAX_KEY_SIZE]; /* supports AES and lower       */
#elif defined(MODULE_TWOFISH)
	uint8_t context[CIPHERS_MAX_KEY_SIZE]; /* supports TwoFish and lower   */
#elif defined(MODULE_SKIPJACK)
	uint8_t context[20];                   /* supports SkipJack and lower  */
#else
	uint8_t context[1];                    /* supports null cipher         */
#endif
} cipher_context_t;


/**
 * @brief   interface for block ciphers
 */
typedef struct cipher_interface_st {
	uint8_t block_size;
	uint8_t max_key_size;
	int (*init)(cipher_context_t* ctx, uint8_t block_size, uint8_t* key,
	            uint8_t key_size);
	int (*encrypt)(cipher_context_t* ctx, uint8_t* plain_block,
	               uint8_t* cipher_block);
	int (*decrypt)(cipher_context_t* ctx, uint8_t* cipher_block,
	               uint8_t* plain_block);
	int (*set_key)(cipher_context_t* ctx, uint8_t* key, uint8_t key_size);
} cipher_interface_t;


/**
 * @brief   numerical ids for each cipher
 */
typedef enum {
	CIPHER_UNKNOWN,
	CIPHER_NULL,
#ifdef MODULE_RC5
	CIPHER_RC5,
#endif
#ifdef MODULE_3DES
	CIPHER_3DES,
#endif
#ifdef MODULE_AES
	CIPHER_AES_128,
#endif
#ifdef MODULE_TWOFISH
	CIPHER_TWOFISH,
#endif
#ifdef MODULE_SKIPJACK
	CIPHER_SKIPJACK
#endif
} cipher_id_t;


/**
 * @brief   cipher entry
 */
typedef struct cipher_entry_st {
	const char* name;
	cipher_id_t id;
	cipher_interface_t* interface;
	uint8_t block_size;
} cipher_entry_t;


/**
 * @brief   list of all supported ciphers
 */
extern const cipher_entry_t cipher_list[];


/**
 * @brief basic struct for using block ciphers
 *        contains the cipher interface and the context
 */
typedef struct {
	cipher_interface_t* interface;
	cipher_context_t context;
} cipher_t;


/**
 * @brief Initialize new cipher state
 *
 * @param cipher     cipher struct to init (already allocated memory)
 * @param cipher_id  cipher algorithm id
 * @param key        encryption key to use
 * @param len        length of the encryption key
 */
int cipher_init(cipher_t* cipher, cipher_id_t cipher_id, uint8_t* key,
                uint8_t key_size);


/**
 * @brief set new encryption key for a cipher
 *
 * @param cipher     cipher struct to use
 * @param key        new encryption key
 * @param len        length of the new encryption key
 */
int cipher_set_key(cipher_t* cipher, uint8_t* key, uint8_t key_size);


/**
 * @brief Encrypt data of BLOCK_SIZE length
 * *
 *
 * @param cipher     Already initialized cipher struct
 * @param input      pointer to input data to encrypt
 * @param output     pointer to allocated memory for encrypted data. It has to
 *                   be of size BLOCK_SIZE
 */
int cipher_encrypt(cipher_t* cipher, uint8_t* input, uint8_t* output);


/**
 * @brief Decrypt data of BLOCK_SIZE length
 * *
 *
 * @param cipher     Already initialized cipher struct
 * @param input      pointer to input data (of size BLOCKS_SIZE) to decrypt
 * @param output     pointer to allocated memory for decrypted data. It has to
 *                   be of size BLOCK_SIZE
 */
int cipher_decrypt(cipher_t* cipher, uint8_t* input, uint8_t* output);


/**
 * @brief Get block size of cipher
 * *
 *
 * @param cipher     Already initialized cipher struct
 */
int cipher_get_block_size(cipher_t* cipher);


/** @} */
#endif /* __CIPHERS_H_ */
