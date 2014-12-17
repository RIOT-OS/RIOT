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
 * @file        cbcmode.h
 * @brief       Headers of the implementation of the CBC Mode of Operation
 *
 * Implementation of the CBC Mode of Operation with Ciphertext-Stealing for encryption.
 *
 * @author      Freie Universitaet Berlin, Computer Systems & Telematics.
 * @author      Nicolai Schmittberger <nicolai.schmittberger@fu-berlin.de>
 * @author      Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 */


#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#ifndef CBCMODE_H_
#define CBCMODE_H_

#include "crypto/ciphers.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MIN(a, b) ( ((a) < (b)) ? (a) : (b))

#define DBG_CRYPTO      1


enum {
    // we allocate some static buffers on the stack; they have to be less
    // than this size
    CBCMODE_MAX_BLOCK_SIZE      = 8,
    CBCMODE_MAX_BLOCK_SIZE_AES  = 16
};

/*// We run a simple state machine in the incremental decrypt:
  //
  //    +--> ONE_BLOCK
  //    |
  // ---|
  //    |
  //    +--> GENERAL --+---> TWO_LEFT_A ----> TWO_LEFT_B
  //            ^      |
  //            |      |
  //            +------+
   */

enum {
    ONE_BLOCK,
    GENERAL,
    TWO_LEFT_A,
    TWO_LEFT_B
};

#define FAIL 0
#define SUCCESS 1


/**
 * @struct CBCModeContext CBCMode.c "CBCMode.c"
 * @brief   The context for processing the en-/decryption in the CBC-Mode with
 *          CTS
 *
 * @param   spill1  test1
 */
typedef struct CBCModeContext {
        // Spill-Block 1 for temporary usage
    uint8_t spill1 [CBCMODE_MAX_BLOCK_SIZE ];
    // Spill-Block 2 for temporary usage
    uint8_t spill2 [CBCMODE_MAX_BLOCK_SIZE ];
    // the blocksize currently used
    uint8_t bsize;
    // how many more bytes of ciphertext do we need to recv
    uint16_t remaining;
    // how many bytes of plaintext we've deciphered.
    uint16_t completed;
    // TRUE iff spill1 is the accumulator and spill2 holds prev cipher text.
    // false o.w.
    uint8_t accum;
    // into the accumulator
    uint8_t offset;
    // state enum
    uint8_t state;
} /*__attribute__ ((packed)) */ CBCModeContext;


/*
 * @brief   Initialize the Mode. It uses the underlying BlockCipher's
 *          preferred block cipher mode, and passes the key and keySize
 *          parameters to the underlying BlockCipher.
 *
 * @param   context   structure to hold the opaque data from this
 *                    initialization call. It should be passed to future
 *                    invocations of this module which use this particular key.
 *                    It also contains the opaque context for the underlying
 *                    BlockCipher as well.
 * @param   keySize   key size in bytes
 * @param   key       pointer to the key
 *
 * @return  Whether initialization was successful. The command may be
 *          unsuccessful if the key size is not valid for the given cipher
 *          implementation. It can also fail if the preferred block size of
 *          the cipher does not agree with the preferred size of the mode.
 */
int block_cipher_mode_init(CipherModeContext *context, uint8_t key_size,
                           uint8_t *key);

/**
 * @brief   same as BlockCipherMode_init but with the possibility to specify
 *          the index of the cipher in the archive
 *
 * @param   context       structure to hold the opaque data from this
 *                        initialization call. It should be passed to future
 *                        invocations of this module which use this particular
 *                        key. It also contains the opaque context for the
 *                        underlying BlockCipher as well.
 * @param   keySize       key size in bytes.
 * @param   key           pointer to the key.
 * @param   cipher_index  the index of the cipher-algorithm to init in the
 *                        (cipher-)archive
 *
 * @return  Whether initialization was successful. The command may be
 *          unsuccessful if the key size is not valid for the given cipher
 *          implementation. It can also fail if the preferred block size of
 *          the cipher does not agree with the preferred size of the mode.
 */
int block_cipher_mode_init0(CipherModeContext *context, uint8_t key_size,
                            uint8_t *key, uint8_t cipher_index);

/**
 * @brief   prints the debug-messages passed by ::dumpBuffer
 *
 * @param   mode      the mode of the debug-message
 * @param   format    pointer to the message
 */
void dbg(uint8_t mode, const char *format, ...);

/**
 * @brief   dumps the passed buffer to the console
 *
 * @param   bufName   pointer to the name of the buffer
 * @param   buf       pointer to the buffer itself
 * @param   size      the size of the buffer in bytes
 */
void dump_buffer(char *bufName, uint8_t *buf, uint8_t size);

/**
 * @brief   Encrypts num_bytes of plaintext blocks (each of size blockSize)
 *          using the key from the init phase. The IV is a pointer to the
 *          initialization vector (of size equal to the blockSize) which is
 *          used to initialize the encryption.
 *          In place encryption should work provided that the plain and and
 *          cipher buffer are the same. (they may either be the same or
 *          non-overlapping. partial overlaps are not supported).
 *
 * @param   plain_blocks  a plaintext block numBlocks, where each block is of
 *                        blockSize bytes
 * @param   cipher_blocks an array of numBlocks * blockSize bytes to hold the
 *                        resulting cyphertext
 * @param   num_bytes     number of data blocks to encrypt
 * @param   IV            an array of the initialization vector. It should be
 *                        of block size bytes
 *
 * @return  Whether the encryption was successful. Possible failure reasons
 *          include not calling init().
 */
int block_cipher_mode_encrypt(CipherModeContext *context, uint8_t *plain_blocks,
                              uint8_t *cipher_blocks, uint16_t num_bytes,
                              uint8_t *IV);

/**
 * @brief   Decrypts num_bytes of ciphertext blocks (each of size blockSize)
 *          using the key from the init phase. The IV is a pointer to the
 *          initialization vector (of size equal to the blockSize) which is
 *          used to initialize the decryption.
 *          In place decryption should work provided that the plain and and
 *          cipher buffer are the same. (they may either be the same or
 *          non-overlapping. partial overlaps are not supported).
 *
 * @param   cipher_blocks  an array of num_bytes * blockSize bytes that holds
 *                         the cipher text
 * @param   plain_blocks   an array of num_bytes * blockSize bytes to hold the
 *                         resulting plaintext.
 * @param   num_bytes      number of data blocks to encrypt
 * @param   IV             an array of the initialization vector. It should be
 *                         of block size bytes
 *
 * @return  Whether the decryption was successful. Possible failure reasons
 *          include not calling init().
 */
int block_cipher_mode_decrypt(CipherModeContext *context,
                              uint8_t *cipher_blocks,
                              uint8_t *plain_blocks,
                              uint16_t num_bytes,
                              uint8_t *IV);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* CBCMODE_H_ */
