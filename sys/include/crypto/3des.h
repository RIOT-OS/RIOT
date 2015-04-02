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
 * @brief       Headers for the implementation of the 3DES cipher-algorithm
 *
 * @author      Freie Universitaet Berlin, Computer Systems & Telematics,
 * @author      Nicolai Schmittberger (nicolai.schmittberger@fu-berlin.de)
 * @author      Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 */

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>

#include "crypto/ciphers.h"

#ifndef THREEDES_H_
#define THREEDES_H_

#ifdef __cplusplus
extern "C" {
#endif

#define THREEDES_BLOCK_SIZE    8
#define THREEDES_MAX_KEY_SIZE 24

#define ROLc(x, y) \
        ((((unsigned long) (x) << (unsigned long) ((y) & 31)) | \
         (((unsigned long) (x) & 0xFFFFFFFFUL) >>               \
           (unsigned long) (32 - ((y) & 31)))) & 0xFFFFFFFFUL)

#define RORc(x, y) \
        (((((unsigned long) (x) & 0xFFFFFFFFUL) >> \
            (unsigned long) ((y) & 31)) | \
           ((unsigned long) (x) << (unsigned long) (32 - ((y) & 31)))) & \
                                  0xFFFFFFFFUL)

//source: http://hostap.epitest.fi/wpa_supplicant/devel/common_8h.html
#define WPA_GET_BE32(a) \
                        ((((uint32_t) (a)[0]) << 24) | \
                         (((uint32_t) (a)[1]) << 16) | \
                         (((uint32_t) (a)[2]) << 8 ) |  ((uint32_t) (a)[3]))

//source: http://hostap.epitest.fi/wpa_supplicant/devel/common_8h.html
#define WPA_PUT_BE32(a, val)                                           \
       do {                                                            \
             (a)[0] = (uint8_t) ((((uint32_t) (val)) >> 24) & 0xff);   \
             (a)[1] = (uint8_t) ((((uint32_t) (val)) >> 16) & 0xff);   \
             (a)[2] = (uint8_t) ((((uint32_t) (val)) >> 8) & 0xff);    \
             (a)[3] = (uint8_t) (((uint32_t)  (val)) & 0xff);          \
        } while (0)




static const uint32_t bytebit[8] = {0200, 0100, 040, 020, 010, 04, 02, 01};

static const uint32_t bigbyte[24] = {
    0x800000UL,  0x400000UL,  0x200000UL,  0x100000UL,
    0x80000UL,   0x40000UL,   0x20000UL,   0x10000UL,
    0x8000UL,    0x4000UL,    0x2000UL,    0x1000UL,
    0x800UL,     0x400UL,     0x200UL,     0x100UL,
    0x80UL,      0x40UL,      0x20UL,      0x10UL,
    0x8UL,       0x4UL,       0x2UL,       0x1L
};
/**
 * @brief   initializes the 3DES Cipher-algorithm with the passed
 *          parameters
 *
 * @param   context     the cipher_context_t-struct to save the
 *                      initialization of the cipher in
 * @param   key         a pointer to the key
 * @param   keySize     the size of the key

 *
 * @return  Whether initialization was successful. The command may be
 *          unsuccessful if the key size is not valid.
 */
int tripledes_init(cipher_context_t *context, const uint8_t* key, uint8_t keySize);

/**
 * @brief   encrypts one plain-block and saves the result in crypt.
 *          encrypts one blocksize long block of plaintext pointed to by
 *          plain to one blocksize long block of ciphertext which will be
 *          written to the the memory-area pointed to by crypt
 *
 * @param   context   the cipher_context_t-struct to use for this
 *                    encryption
 * @param   plain     a pointer to the plaintext-block (of size
 *                    blocksize)
 * @param   crypt     a pointer to the place where the ciphertext will
 *                    be stored
 *
 * @return  -1 if no space for the key could be malloced
 *                      -2 if the key could not be setup correctly
 *                       1 if encryption was successful
 */
int tripledes_encrypt(const cipher_context_t *context, const uint8_t *plain, uint8_t *crypt);

/**
 * @brief  decrypts one cipher-block and saves the plain-block in plain.
 *         decrypts one blocksize long block of ciphertext pointed to by
 *         crypt to one blocksize long block of plaintext and stores the
 *         plaintext in the memory-area pointed to by plain
 *
 * @param   context   the cipher_context_t-struct to use for this
 *                    decryption
 * @param   crypt     a pointer to the ciphertext-block (of size blocksize)
 *                    to be decrypted
 * @param   plain     a pointer to the place where the decrypted plaintext
 *                    will be stored
 *
 * @return  -1 if no space for the key could be malloced
 *          -2 if the key could not be setup correctly
 *           1 if decryption was successful
 */
int tripledes_decrypt(const cipher_context_t *context, const uint8_t *crypt, uint8_t *plain);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* THREEDES_H_ */
