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
 * @brief       Basic header for all encryption algorithms.
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

/// the length of keys in bytes
#define PARSEC_MAX_BLOCK_CIPHERS  5
#define CIPHERS_KEYSIZE           20

/**
 * @brief   the context for cipher-operations
 *          always order by number of bytes descending!!! <br>
 * rc5          needs 104 bytes                           <br>
 * threedes     needs 24  bytes                           <br>
 * aes          needs PARSEC_KEYSIZE bytes                <br>
 * twofish      needs PARSEC_KEYSIZE bytes                <br>
 * skipjack     needs 20 bytes                            <br>
 * identity     needs 1  byte                             <br>
 */
typedef struct {
#if defined(MODULE_RC5)
    uint8_t context[104];             /* supports RC5 and lower       */
#elif defined(MODULE_3DES)
    uint8_t context[24];              /* supports TripleDES and lower */
#elif defined(MODULE_AES)
    uint8_t context[CIPHERS_KEYSIZE]; /* supports AES and lower       */
#elif defined(MODULE_TWOFISH)
    uint8_t context[CIPHERS_KEYSIZE]; /* supports TwoFish and lower   */
#elif defined(MODULE_SKIPJACK)
    uint8_t context[20];              /* supports SkipJack and lower  */
#endif
} cipher_context_t;


/**
 * @struct BlockCipherInterface_t
 * @brief   BlockCipher-Interface for the Cipher-Algorithms
 * @typedef BlockCipherInterface_t
 */
typedef struct {
    char name[10];
    // the init function
    int (*BlockCipher_init)(cipher_context_t *context, uint8_t blockSize,
                            uint8_t keySize, uint8_t *key);
    // the encrypt function
    int (*BlockCipher_encrypt)(cipher_context_t *context, uint8_t *plainBlock,
                               uint8_t *cipherBlock);
    // the decrypt function
    int (*BlockCipher_decrypt)(cipher_context_t *context, uint8_t *cipherBlock,
                               uint8_t *plainBlock);
    // the setupKey function
    int (*setupKey)(cipher_context_t *context, uint8_t *key, uint8_t keysize);
    // read the BlockSize of this Cipher
    uint8_t (*BlockCipherInfo_getPreferredBlockSize)(void);
} block_cipher_interface_t;


/** @} */
#endif /* __CIPHERS_H_ */
