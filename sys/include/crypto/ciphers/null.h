/*
 * Copyright (C) 2014 Freie Universität Berlin, Computer Systems & Telematics
 *
 * This source code is licensed under the LGPLv2 license,
 * See the file LICENSE for more details.
 */

/**
 * @ingroup     sys_crypto
 * @{
 *
 * @file        null.h
 * @brief       null cipher.
 *
 * @author      Freie Universitaet Berlin, Computer Systems & Telematics
 * @author      Nico von Geyso <nico.geyso@fu-berlin.de>
 */

#ifndef __CRYPTO_NULL_H_
#define __CRYPTO_NULL_H_

#include <stdint.h>
#include "crypto/ciphers.h"

#define NULL_BLOCK_SIZE 16
#define NULL_KEY_SIZE 0


/**
 * @brief the cipher_context_t-struct adapted for AES
 */
typedef struct {
	uint8_t size;
} null_context_t;


/**
 * @brief   initializes the NULL cipher
 *
 * @param       context    the cipher_context_t-struct to save the initialization
 *                         of the cipher in
 * @param       block_size the used blocksize
 * @param       key        a pointer to the key, has to be NULL
 * @param       key_size   the size of the key, has to be zero
 *
 * @return  0 if blocksize doesn't match else 1
 */
int null_init(cipher_context_t* context, uint8_t block_size, uint8_t* key,
              uint8_t key_size);


/**
 * @brief   The null cipher does not encrypt anything. It just copies in to out.
 *
 * @param       context      the cipher_context_t-struct to use for this
 *                           encryption
 * @param       plainBlock   a pointer to the plaintext-block (of size
 *                           blocksize)
 * @param       cipherBlock  a pointer to the place where the ciphertext will
 *                           be stored
 *
 * @return  0 or -1 if in/out was a NULL pointer
 */
int null_encrypt(cipher_context_t* context, uint8_t* in, uint8_t* out);


/**
 * @brief Decrypt data of BLOCK_SIZE length
 * *
 *
 * @param cipher     Already initialized cipher struct
 * @param input      pointer to input data (of size BLOCKS_SIZE) to decrypt
 * @param output     pointer to allocated memory for decrypted data. It has to
 *                   be of size BLOCK_SIZE
 */
static inline int null_decrypt(cipher_context_t* context, uint8_t* in, uint8_t* out)
{
	return null_encrypt(context, in, out);
}


/**
 * @brief   for the null cipher you can not set a key. this is a dummy function
 *          to fulfill api convention.
 *
 * @param       context    the initialized cipher_context_t-struct
 * @param       key        a pointer to the key, has to be NULL
 * @param       key_size   the length of the key, has to be zero
 *
 * @return  0 if initialized blocksize is wrong, 1 else
 */
int null_set_key(cipher_context_t* context, uint8_t* key, uint8_t key_size);

#endif /* __CRYPTO_NULL_H_*/

