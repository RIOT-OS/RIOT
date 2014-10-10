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
 * @file        ciphers.h
 * @brief       Headers for the packet encryption class. They are used to encrypt single packets.
 *
 * @author      Freie Universitaet Berlin, Computer Systems & Telematics
 * @author      Nicolai Schmittberger <nicolai.schmittberger@fu-berlin.de>
 * @author      Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 * @author      Mark Essien <markessien@gmail.com>
 */

#ifndef __CIPHERS_H_
#define __CIPHERS_H_

#ifdef __cplusplus
extern "C" {
#endif

/* Shared header file for all cipher algorithms */

/* Set the algorithms that should be compiled in here. When these defines
 * are set, then packets will be compiled 5 times.
 *
 * */
#define AES
// #define RC5
// #define THREEDES
// #define AES
// #define TWOFISH
// #define SKIPJACK

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
#if defined(RC5)
    uint8_t context[104];             // supports RC5 and lower
#elif defined(THREEDES)
    uint8_t context[24];              // supports ThreeDES and lower
#elif defined(AES)
    uint8_t context[CIPHERS_KEYSIZE]; // supports AES and lower
#elif defined(TWOFISH)
    uint8_t context[CIPHERS_KEYSIZE]; // supports TwoFish and lower
#elif defined(SKIPJACK)
    uint8_t context[20];              // supports SkipJack and lower
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


typedef struct CipherModeContext {
    cipher_context_t cc;            // CipherContext for the cipher-operations
    uint8_t context[24];         // context for the block-cipher-modes'
                                 // internal functions
    //CBCModeContext* context;
} CipherModeContext;


/**
 * @brief       struct for an archive of all available ciphers
 * @struct      BlockCipher_Archive_t CipherManager.h "ciphers/CipherManager.h"
 * @typedef     BlockCipher_Archive_t
 */
typedef struct {
        // the number of available ciphers
    uint8_t NoCiphers;
    // the ciphers in form or BlockCipherInterface_ts
    block_cipher_interface_t ciphers[PARSEC_MAX_BLOCK_CIPHERS];
} block_cipher_archive_t;

typedef struct {
        // cipher_context_t for the cipher-operations
    cipher_context_t cc;
#if defined(AES) || defined (TWOFISH)
    // supports 16-Byte blocksize
    uint8_t context[20];
#else
    // supports 8-Byte blocksize
    uint8_t context[12];
#endif
} cipher_mac_context_t;

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* __CIPHERS_H_ */
