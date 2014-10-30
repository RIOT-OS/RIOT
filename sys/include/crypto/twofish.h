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
 * @file        twofish.h
 * @brief       Headers for the implementation of the TwoFish Cipher-Algorithm
 *
 * @author      Freie Universitaet Berlin, Computer Systems & Telematics
 * @author      Nicolai Schmittberger <nicolai.schmittberger@fu-berlin.de>
 * @author      Zakaria Kasmi <zkasmi@inf.fu-berlin.de> *
 */

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "crypto/ciphers.h"

#ifndef TWOFISH_H_
#define TWOFISH_H_

#define TWOFISH_BLOCK_SIZE      16
#define TWOFISH_KEY_SIZE        16   //only alternative is 32!



/**
 * @brief  Structure for an expanded Twofish key.
 *
 * @param   s   contains the key-dependent S-boxes composed with the MDS
 *              matrix;
 * @param   w   contains the eight "whitening" subkeys, K[0] through K[7].
 * @param   k   holds the remaining, "round" subkeys.
 *
 * Note that k[i] corresponds to what the Twofish paper calls K[i+8].
 */
typedef struct {
    uint32_t s[4][256], w[8], k[32];
} twofish_context_t;


/**
 * @brief   Initialize the TwoFish-BlockCipher context.
 *
 * @param   context     structure to hold the opaque data from this
 *                      initialization
 *                      call. It should be passed to future invocations of
 *                      this module
 *                      which use this particular key.
 * @param   blockSize   size of the block in bytes.
 * @param   keySize     key size in bytes
 * @param   key         pointer to the key
 *
 * @return  Whether initialization was successful. The command may be
 *         unsuccessful if the key size or blockSize are not valid.
 */
int twofish_init(cipher_context_t *context, uint8_t block_size, uint8_t key_size, uint8_t *key);

/**
 * @brief   Sets up the context to use the passed key for usage with TwoFish
 *          Performs the key expansion on the real secret.
 *
 * @param   context     the CipherContext-struct to save the updated key in
 * @param   key         a pointer to the secret key
 * @param   keysize     the length of the secret key
 *
 * @return SUCCESS
 */
int twofish_setup_key(cipher_context_t *context, uint8_t *key, uint8_t key_size);

/**
 * @brief   Encrypts a single block (of blockSize) using the passed context.
 *
 * @param   context   holds the module specific opaque data related to the
 *                    key (perhaps key expansions).
 * @param   in        a plaintext block of blockSize
 * @param   out       the resulting ciphertext block of blockSize
 *
 * @return  Whether the encryption was successful. Possible failure reasons
 *          include not calling init().
 */
int twofish_encrypt(cipher_context_t *context, uint8_t *in, uint8_t *out);

/**
 * @brief   Decrypts a single block (of blockSize) using the passed context.
 *
 * @param   context   holds the module specific opaque data related to the
 *                    key (perhaps key expansions).
 * @param   in        a ciphertext block of blockSize
 * @param   out       the resulting plaintext block of blockSize
 *
 * @return  Whether the decryption was successful. Possible failure reasons
 *          include not calling init()
 */
int twofish_decrypt(cipher_context_t *context, uint8_t *in, uint8_t *out);

/**
 * @brief   Returns the preferred block size that this cipher operates with.
 *          It is always safe to call this function before the init() call has
 *          been made.
 *
 * @return  the preferred block size for this cipher. In the case where the
 *          cipher operates with multiple block sizes, this will pick one
 *          particular size (deterministically).
 */
uint8_t twofish_get_preferred_block_size(void);

/**
 * Interface to access the functions
 *
 */
extern block_cipher_interface_t twofish_interface;

/** @} */
#endif /* TWOFISH_H_ */
