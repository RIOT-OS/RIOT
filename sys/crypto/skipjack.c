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
 * @file        skipjack.c
 * @brief       implementation of the SkipJack Cipher-Algorithm
 *
 * @author      Freie Universitaet Berlin, Computer Systems & Telematics
 * @author      Nicolai Schmittberger <nicolai.schmittberger@fu-berlin.de>
 * @author      Zakaria Kasmi <zkasmi@inf.fu-berlin.de>
 * @author      Naveen Sastry
 *
 * @}
 */

/*
 * From the NIST description of SkipJack.
 */
// our context: we just expand the key to 20 bytes.
//
// we have two options for the expansion:
//   1. no expansion. advantage: 10byte context. disadvantage: mucks up
//      the G box code with ifs / mods. Alternatively adds lots of code and
//      muckiness.
//   2. expand key to 128 bytes. Makes G boxes easy to write, and minimal
//      code expansion. disadvantage: wasted memory
//   3. expand key to 20 bytes.  G boxes still simple, the encode and decode
//      functions are a little more complicated, but still more or less
//      managable. this is what we've implemented.

#include <stdio.h>
#include <stdarg.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include "crypto/ciphers.h"
#include "crypto/skipjack.h"


/**
 * @brief Define a fixed block size of 8 bytes
 */
#define BLOCK_SIZE          (8U)

/**
 * @brief Interface to the skipjack cipher
 */
block_cipher_interface_t skipjack_interface = {
    "SkipJack",
    skipjack_init,
    skipjack_encrypt,
    skipjack_decrypt,
    skipjack_setup_key,
    skipjack_get_preferred_block_size
};

// F-BOX
// It can live in either RAM (faster access) or program memory (save ram,
// but slower access). The type CRYPTO_TABLE_TYPE, defined in crypto.h
// defines where we drop the table and how we access it.  This is necessary
// to compile for the PC target since it doesn't support tables in
// program memory the same way.
static const uint8_t SJ_F[] /*__attribute__((C))*/ = {
    0xA3, 0xD7, 0x09, 0x83, 0xF8, 0x48, 0xF6, 0xF4, 0xB3, 0x21, 0x15, 0x78,
    0x99, 0xB1, 0xAF, 0xF9, 0xE7, 0x2D, 0x4D, 0x8A, 0xCE, 0x4C, 0xCA, 0x2E,
    0x52, 0x95, 0xD9, 0x1E, 0x4E, 0x38, 0x44, 0x28, 0x0A, 0xDF, 0x02, 0xA0,
    0x17, 0xF1, 0x60, 0x68, 0x12, 0xB7, 0x7A, 0xC3, 0xE9, 0xFA, 0x3D, 0x53,
    0x96, 0x84, 0x6B, 0xBA, 0xF2, 0x63, 0x9A, 0x19, 0x7C, 0xAE, 0xE5, 0xF5,
    0xF7, 0x16, 0x6A, 0xA2, 0x39, 0xB6, 0x7B, 0x0F, 0xC1, 0x93, 0x81, 0x1B,
    0xEE, 0xB4, 0x1A, 0xEA, 0xD0, 0x91, 0x2F, 0xB8, 0x55, 0xB9, 0xDA, 0x85,
    0x3F, 0x41, 0xBF, 0xE0, 0x5A, 0x58, 0x80, 0x5F, 0x66, 0x0B, 0xD8, 0x90,
    0x35, 0xD5, 0xC0, 0xA7, 0x33, 0x06, 0x65, 0x69, 0x45, 0x00, 0x94, 0x56,
    0x6D, 0x98, 0x9B, 0x76, 0x97, 0xFC, 0xB2, 0xC2, 0xB0, 0xFE, 0xDB, 0x20,
    0xE1, 0xEB, 0xD6, 0xE4, 0xDD, 0x47, 0x4A, 0x1D, 0x42, 0xED, 0x9E, 0x6E,
    0x49, 0x3C, 0xCD, 0x43, 0x27, 0xD2, 0x07, 0xD4, 0xDE, 0xC7, 0x67, 0x18,
    0x89, 0xCB, 0x30, 0x1F, 0x8D, 0xC6, 0x8F, 0xAA, 0xC8, 0x74, 0xDC, 0xC9,
    0x5D, 0x5C, 0x31, 0xA4, 0x70, 0x88, 0x61, 0x2C, 0x9F, 0x0D, 0x2B, 0x87,
    0x50, 0x82, 0x54, 0x64, 0x26, 0x7D, 0x03, 0x40, 0x34, 0x4B, 0x1C, 0x73,
    0xD1, 0xC4, 0xFD, 0x3B, 0xCC, 0xFB, 0x7F, 0xAB, 0xE6, 0x3E, 0x5B, 0xA5,
    0xAD, 0x04, 0x23, 0x9C, 0x14, 0x51, 0x22, 0xF0, 0x29, 0x79, 0x71, 0x7E,
    0xFF, 0x8C, 0x0E, 0xE2, 0x0C, 0xEF, 0xBC, 0x72, 0x75, 0x6F, 0x37, 0xA1,
    0xEC, 0xD3, 0x8E, 0x62, 0x8B, 0x86, 0x10, 0xE8, 0x08, 0x77, 0x11, 0xBE,
    0x92, 0x4F, 0x24, 0xC5, 0x32, 0x36, 0x9D, 0xCF, 0xF3, 0xA6, 0xBB, 0xAC,
    0x5E, 0x6C, 0xA9, 0x13, 0x57, 0x25, 0xB5, 0xE3, 0xBD, 0xA8, 0x3A, 0x01,
    0x05, 0x59, 0x2A, 0x46
};


int skipjack_init(cipher_context_t *context, uint8_t blockSize, uint8_t keySize,
                  uint8_t *key)
{
    // 8 byte blocks only
    if (blockSize != BLOCK_SIZE) {
        return 0;
    }

    return skipjack_setup_key(context, key, keySize);
}

/**
 * @brief convert 2x uint8_t to uint16_t
 *
 * @param c     pointer to the 2x uint8_t input
 * @param s     pointer to the resulting uint16_t
 *
 */
static void c2sM(uint8_t *c, uint16_t *s)
{
    memcpy(s, c, sizeof(uint16_t));
    return;
}

/**
 * @brief convert one uint16_t to 2x uint8_t
 *
 * @param s pointer to the uint16_t input
 * @param c pointer to the first resulting uint8_ts
 */
static void s2cM(uint16_t s, uint8_t *c)
{
    memcpy(c, &s, sizeof(uint16_t));
    return;
}


int skipjack_encrypt(cipher_context_t *context, uint8_t *plainBlock,
                     uint8_t *cipherBlock)
{

    // prologue 10 pushs = 20 cycles
    /*register*/ uint8_t counter = 1;
    skipjack_context_t *skipjack_context = (skipjack_context_t *)context->context;
    /*register*/ uint8_t *skey  = skipjack_context->skey;
    /*register*/ uint16_t w1, w2, w3, w4, tmp;
    /*register*/ uint8_t bLeft, bRight;

    //dumpBuffer("SkipJack.encrypt: plainBlock", plainBlock, 8);

    c2sM(plainBlock, &w1);
    plainBlock += 2;
    c2sM(plainBlock, &w2);
    plainBlock += 2;
    c2sM(plainBlock, &w3);
    plainBlock += 2;
    c2sM(plainBlock, &w4);
    plainBlock += 2;

    /*
     * code if we had expanded key to 128 bytes. this is what the code below
     * does, but after every 5 operations, it resets the where we are
     * in the key back to the beginning of the skey. so our loops end up
     * looking a little funny.
     *
     *  while (counter < 9)
     *   RULE_A(skey, w1, w2, w3, w4, counter, tmp, bLeft, bRight );
     *  while (counter < 17)
     *   RULE_B(skey, w1, w2, w3, w4, counter, tmp, bLeft, bRight );
     *  while (counter < 25)
     *   RULE_A(skey, w1, w2, w3, w4, counter, tmp, bLeft, bRight );
     *  while (counter < 33)
     *   RULE_B(skey, w1, w2, w3, w4, counter, tmp, bLeft, bRight );
     */

    while (counter < 6) { // 5x
        RULE_A(skey, w1, w2, w3, w4, counter, tmp, bLeft, bRight);
    }

    skey = skipjack_context->skey;

    while (counter < 9) { // 3x
        RULE_A(skey, w1, w2, w3, w4, counter, tmp, bLeft, bRight);
    }

    while (counter < 11) { // 2x
        RULE_B(skey, w1, w2, w3, w4, counter, tmp, bLeft, bRight);
    }

    skey = skipjack_context->skey;

    while (counter < 16) { // 5x
        RULE_B(skey, w1, w2, w3, w4, counter, tmp, bLeft, bRight);
    }

    skey = skipjack_context->skey;
    // 1x
    RULE_B(skey, w1, w2, w3, w4, counter, tmp, bLeft, bRight);

    while (counter < 21) { // 4x
        RULE_A(skey, w1, w2, w3, w4, counter, tmp, bLeft, bRight);
    }

    skey = skipjack_context->skey;

    while (counter < 25) { // 4x
        RULE_A(skey, w1, w2, w3, w4, counter, tmp, bLeft, bRight);
    }

    // 1x
    RULE_B(skey, w1, w2, w3, w4, counter, tmp, bLeft, bRight);
    skey = skipjack_context->skey;

    while (counter < 31) { // 5x
        RULE_B(skey, w1, w2, w3, w4, counter, tmp, bLeft, bRight);
    }

    skey = skipjack_context->skey;

    while (counter < 33) { // 2x
        RULE_B(skey, w1, w2, w3, w4, counter, tmp, bLeft, bRight);
    }

    s2cM(w1, cipherBlock);
    cipherBlock += 2;
    s2cM(w2, cipherBlock);
    cipherBlock += 2;
    s2cM(w3, cipherBlock);
    cipherBlock += 2;
    s2cM(w4, cipherBlock);
    cipherBlock += 2;

    return 1;
}


int skipjack_decrypt(cipher_context_t *context, uint8_t *cipherBlock,
                     uint8_t *plainBlock)
{
    /*register*/ uint8_t counter = 32;
    skipjack_context_t *skipjack_context = (skipjack_context_t *)context->context;
    /*register*/ uint8_t *skey  = skipjack_context->skey + 4;
    /*register*/ uint16_t w1, w2, w3, w4, tmp;
    /*register*/ uint8_t bLeft, bRight;

    //dumpBuffer("SkipJack.decrypt: cipherBlock", cipherBlock, 8);

    c2sM(cipherBlock, &w1);
    cipherBlock += 2;
    c2sM(cipherBlock, &w2);
    cipherBlock += 2;
    c2sM(cipherBlock, &w3);
    cipherBlock += 2;
    c2sM(cipherBlock, &w4);

    /*
      // code if we had expanded key to 128 bytes. this is what the code below
      // does, but after every 5 operations, it resets the where we are
      // in the key back to the beginning of the skey. so our loops end up
      // looking a little funny.

    while (counter > 24)
      RULE_B_INV(skey, w1, w2, w3, w4, counter, tmp, bLeft, bRight );
    while (counter > 16)
      RULE_A_INV(skey, w1, w2, w3, w4, counter, tmp, bLeft, bRight );
    while (counter > 8)
      RULE_B_INV(skey, w1, w2, w3, w4, counter, tmp, bLeft, bRight );
    while (counter > 0)
      RULE_A_INV(skey, w1, w2, w3, w4, counter, tmp, bLeft, bRight );
    */

    while (counter > 30) { //2x
        RULE_B_INV(skey, w1, w2, w3, w4, counter, tmp, bLeft, bRight);
    }

    skey  = skipjack_context->skey + 16;

    while (counter > 25) { //5x
        RULE_B_INV(skey, w1, w2, w3, w4, counter, tmp, bLeft, bRight);
    }

    skey  = skipjack_context->skey + 16;
    //1x
    RULE_B_INV(skey, w1, w2, w3, w4, counter, tmp, bLeft, bRight);

    while (counter > 20) { //4x
        RULE_A_INV(skey, w1, w2, w3, w4, counter, tmp, bLeft, bRight);
    }

    skey  = skipjack_context->skey + 16;

    while (counter > 16) { //4x
        RULE_A_INV(skey, w1, w2, w3, w4, counter, tmp, bLeft, bRight);
    }

    //1x
    RULE_B_INV(skey, w1, w2, w3, w4, counter, tmp, bLeft, bRight);
    skey  = skipjack_context->skey + 16;

    while (counter > 10) { //5x
        RULE_B_INV(skey, w1, w2, w3, w4, counter, tmp, bLeft, bRight);
    }

    skey  = skipjack_context->skey + 16;

    while (counter > 8) { // 2x
        RULE_B_INV(skey, w1, w2, w3, w4, counter, tmp, bLeft, bRight);
    }

    while (counter > 5) { // 3x
        RULE_A_INV(skey, w1, w2, w3, w4, counter, tmp, bLeft, bRight);
    }

    skey  = skipjack_context->skey + 16;

    while (counter > 0) { // 5x
        RULE_A_INV(skey, w1, w2, w3, w4, counter, tmp, bLeft, bRight);
    }

    s2cM(w1, plainBlock);
    plainBlock += 2;
    s2cM(w2, plainBlock);
    plainBlock += 2;
    s2cM(w3, plainBlock);
    plainBlock += 2;
    s2cM(w4, plainBlock);

    return 1;
}


int skipjack_setup_key(cipher_context_t *context, uint8_t *key, uint8_t keysize)
{
    int i = 0;
    skipjack_context_t *skipjack_context = (skipjack_context_t *)context->context;
    uint8_t *skey = skipjack_context->skey;

    // for keys which are smaller than 160 bits, concatenate until they reach
    // 160 bits in size. Note that key expansion is just concatenation.
    if (keysize < CIPHERS_KEYSIZE) {
        //fill up by concatenating key to as long as needed
        for (i = 0; i < CIPHERS_KEYSIZE; i++) {
            skey[i] = key[(i % keysize)];
        }
    }
    else {
        for (i = 0; i < CIPHERS_KEYSIZE; i++) {
            skey[i] = key[i];
        }
    }

    return 1;
}


uint8_t skipjack_get_preferred_block_size(void)
{
    return BLOCK_SIZE;
}
