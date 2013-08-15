/*
 * Ciphers.h - The headers for the packet encryption class.
 *
 * Copyright (C) 2013 Freie Universität Berlin, Computer Systems & Telematics
 *
 * This source code is licensed under the LGPLv2 license,
 * See the file LICENSE for more details.
 */


/**
 * @file        Ciphers.h
 * @brief       Headers for the packet encryption class.
 *              they are used to encrypt single packets.
 *
 * @author      Freie Universitaet Berlin, Computer Systems & Telematics
 * @author      Nicolai Schmittberger <nicolai.schmittberger@fu-berlin.de>
 * @author      Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 * @author      Mark Essien <markessien@gmail.com>
 *
 * @date        20.09.2013 17:13:23
 *
 */

#ifndef __CIPHERS_H_
#define __CIPHERS_H_

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
typedef struct CipherContext {
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
} CipherContext;


/**
 * @struct BlockCipherInterface_t
 * @brief   BlockCipher-Interface for the Cipher-Algorithms
 * @typedef BlockCipherInterface_t
 */
typedef struct BlockCipherInterface_t {
    char name[10];
    // the init function
    int (*BlockCipher_init)(CipherContext *context, uint8_t blockSize,
    		                uint8_t keySize, uint8_t *key);
    // the encrypt function
    int (*BlockCipher_encrypt)(CipherContext *context, uint8_t *plainBlock,
    		                   uint8_t *cipherBlock);
    // the decrypt function
    int (*BlockCipher_decrypt)(CipherContext *context, uint8_t *cipherBlock,
    		                   uint8_t *plainBlock);
    // the setupKey function
    int (*setupKey)(CipherContext *context, uint8_t *key, uint8_t keysize);
    // read the BlockSize of this Cipher
    uint8_t (*BlockCipherInfo_getPreferredBlockSize)(void);
} block_cipher_interface_t;


typedef struct CipherModeContext {
    CipherContext cc;            // CipherContext for the cipher-operations
    uint8_t context[24];         // context for the block-cipher-modes'
                                 // internal functions
    //CBCModeContext* context;
} CipherModeContext;


/**
 * @brief       struct for an archive of all available ciphers
 * @struct      BlockCipher_Archive_t CipherManager.h "ciphers/CipherManager.h"
 * @typedef     BlockCipher_Archive_t
 */
typedef struct BlockCipher_Archive_t {
	// the number of available ciphers
    uint8_t NoCiphers;
    // the ciphers in form or BlockCipherInterface_ts
    block_cipher_interface_t ciphers[PARSEC_MAX_BLOCK_CIPHERS];
} BlockCipher_Archive_t;

typedef struct MACContext {
	// CipherContext for the cipher-operations
    CipherContext cc;
#if defined(AES) || defined (TWOFISH)
    // supports 16-Byte blocksize
    uint8_t context[20];
#else
    // supports 8-Byte blocksize
    uint8_t context[12];
#endif
} MACContext;

#endif
