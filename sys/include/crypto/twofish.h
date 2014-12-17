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

#ifdef __cplusplus
extern "C" {
#endif

#define TWOFISH_BLOCK_SIZE      16
#define TWOFISH_KEY_SIZE        16   //only alternative is 32!

/**
 * Macro to perform one column of the RS matrix multiplication.  The
 * parameters a, b, c, and d are the four bytes of output; i is the index
 * of the key bytes, and w, x, y, and z, are the column of constants from
 * the RS matrix, preprocessed through the poly_to_exp table.
 **/
#define CALC_S(a, b, c, d, i, w, x, y, z) \
   if (key[i]) { \
      tmp = poly_to_exp[key[i] - 1]; \
      (a) ^= exp_to_poly[tmp + (w)]; \
      (b) ^= exp_to_poly[tmp + (x)]; \
      (c) ^= exp_to_poly[tmp + (y)]; \
      (d) ^= exp_to_poly[tmp + (z)]; \
   }

/**
 * Macros to calculate the key-dependent S-boxes for a 128-bit key using
 * the S vector from CALC_S.  CALC_SB_2 computes a single entry in all
 * four S-boxes, where i is the index of the entry to compute, and a and b
 * are the index numbers preprocessed through the q0 and q1 tables
 * respectively.  CALC_SB is simply a convenience to make the code shorter;
 * it calls CALC_SB_2 four times with consecutive indices from i to i+3,
 * using the remaining parameters two by two.
 **/

#define CALC_SB_2(i, a, b) \
   ctx->s[0][i] = mds[0][q0[(a) ^ sa] ^ se]; \
   ctx->s[1][i] = mds[1][q0[(b) ^ sb] ^ sf]; \
   ctx->s[2][i] = mds[2][q1[(a) ^ sc] ^ sg]; \
   ctx->s[3][i] = mds[3][q1[(b) ^ sd] ^ sh]

#define CALC_SB(i, a, b, c, d, e, f, g, h) \
   CALC_SB_2 (i, a, b); CALC_SB_2 ((i)+1, c, d); \
   CALC_SB_2 ((i)+2, e, f); CALC_SB_2 ((i)+3, g, h)

/* Macros exactly like CALC_SB and CALC_SB_2, but for 256-bit keys. */

#define CALC_SB256_2(i, a, b) \
   ctx->s[0][i] = mds[0][q0[q0[q1[(b) ^ sa] ^ se] ^ si] ^ sm]; \
   ctx->s[1][i] = mds[1][q0[q1[q1[(a) ^ sb] ^ sf] ^ sj] ^ sn]; \
   ctx->s[2][i] = mds[2][q1[q0[q0[(a) ^ sc] ^ sg] ^ sk] ^ so]; \
   ctx->s[3][i] = mds[3][q1[q1[q0[(b) ^ sd] ^ sh] ^ sl] ^ sp];

#define CALC_SB256(i, a, b, c, d, e, f, g, h) \
   CALC_SB256_2 (i, a, b); CALC_SB256_2 ((i)+1, c, d); \
   CALC_SB256_2 ((i)+2, e, f); CALC_SB256_2 ((i)+3, g, h)

/**
 * Macros to calculate the whitening and round subkeys.  CALC_K_2 computes the
 * last two stages of the h() function for a given index (either 2i or 2i+1).
 * a, b, c, and d are the four bytes going into the last two stages.  For
 * 128-bit keys, this is the entire h() function and a and c are the index
 * preprocessed through q0 and q1 respectively; for longer keys they are the
 * output of previous stages.  j is the index of the first key byte to use.
 * CALC_K computes a pair of subkeys for 128-bit Twofish, by calling CALC_K_2
 * twice, doing the Psuedo-Hadamard Transform, and doing the necessary
 * rotations.  Its parameters are: a, the array to write the results into,
 * j, the index of the first output entry, k and l, the preprocessed indices
 * for index 2i, and m and n, the preprocessed indices for index 2i+1.
 * CALC_K256_2 expands CALC_K_2 to handle 256-bit keys, by doing two
 * additional lookup-and-XOR stages.  The parameters a and b are the index
 * preprocessed through q0 and q1 respectively; j is the index of the first
 * key byte to use.  CALC_K256 is identical to CALC_K but for using the
 * CALC_K256_2 macro instead of CALC_K_2.
 **/

#define CALC_K_2(a, b, c, d, j) \
     mds[0][q0[a ^ key[(j) + 8]] ^ key[j]]        \
   ^ mds[1][q0[b ^ key[(j) + 9]] ^ key[(j) + 1]]  \
   ^ mds[2][q1[c ^ key[(j) + 10]] ^ key[(j) + 2]] \
   ^ mds[3][q1[d ^ key[(j) + 11]] ^ key[(j) + 3]]

#define CALC_K(a, j, k, l, m, n) \
   x = CALC_K_2 (k, l, k, l, 0); \
   y = CALC_K_2 (m, n, m, n, 4); \
   y = (y << 8) + (y >> 24);     \
   x += y; y += x; ctx->a[j] = x; \
   ctx->a[(j) + 1] = (y << 9) + (y >> 23)

#define CALC_K256_2(a, b, j) \
   CALC_K_2 (q0[q1[b ^ key[(j) + 24]] ^ key[(j) + 16]], \
             q1[q1[a ^ key[(j) + 25]] ^ key[(j) + 17]], \
             q0[q0[a ^ key[(j) + 26]] ^ key[(j) + 18]], \
             q1[q0[b ^ key[(j) + 27]] ^ key[(j) + 19]], j)

#define CALC_K256(a, j, k, l, m, n) \
   x = CALC_K256_2 (k, l, 0); \
   y = CALC_K256_2 (m, n, 4); \
   y = (y << 8) + (y >> 24); \
   x += y; y += x; ctx->a[j] = x; \
   ctx->a[(j) + 1] = (y << 9) + (y >> 23)


/**
 * Macros to compute the g() function in the encryption and decryption
 * rounds.  G1 is the straight g() function; G2 includes the 8-bit
 * rotation for the high 32-bit word.
 **/

#define G1(a) \
     (ctx->s[0][(a) & 0xFF]) ^ (ctx->s[1][((a) >> 8) & 0xFF]) \
   ^ (ctx->s[2][((a) >> 16) & 0xFF]) ^ (ctx->s[3][(a) >> 24])

#define G2(b) \
     (ctx->s[1][(b) & 0xFF]) ^ (ctx->s[2][((b) >> 8) & 0xFF]) \
   ^ (ctx->s[3][((b) >> 16) & 0xFF]) ^ (ctx->s[0][(b) >> 24])

/**
 * Encryption and decryption Feistel rounds.  Each one calls the two g()
 * macros, does the PHT, and performs the XOR and the appropriate bit
 * rotations.  The parameters are the round number (used to select subkeys),
 * and the four 32-bit chunks of the text.
 **/

#define ENCROUND(n, a, b, c, d) \
   x = G1 (a); y = G2 (b); \
   x += y; y += x + ctx->k[2 * (n) + 1]; \
   (c) ^= x + ctx->k[2 * (n)]; \
   (c) = ((c) >> 1) + ((c) << 31); \
   (d) = (((d) << 1)+((d) >> 31)) ^ y

#define DECROUND(n, a, b, c, d) \
   x = G1 (a); y = G2 (b); \
   x += y; y += x; \
   (d) ^= y + ctx->k[2 * (n) + 1]; \
   (d) = ((d) >> 1) + ((d) << 31); \
   (c) = (((c) << 1)+((c) >> 31)); \
   (c) ^= (x + ctx->k[2 * (n)])

/**
 * Encryption and decryption cycles; each one is simply two Feistel rounds
 * with the 32-bit chunks re-ordered to simulate the "swap"
 **/

#define ENCCYCLE(n) \
   ENCROUND (2 * (n), a, b, c, d); \
   ENCROUND (2 * (n) + 1, c, d, a, b)

#define DECCYCLE(n) \
   DECROUND (2 * (n) + 1, c, d, a, b); \
   DECROUND (2 * (n), a, b, c, d)

/**
 * Macros to convert the input and output bytes into 32-bit words,
 * and simultaneously perform the whitening step.  INPACK packs word
 * number n into the variable named by x, using whitening subkey number m.
 * OUTUNPACK unpacks word number n from the variable named by x, using
 * whitening subkey number m.
 **/

#define INPACK(n, x, m) \
   x = in[4 * (n)] ^ (in[4 * (n) + 1] << 8) \
     ^ (in[4 * (n) + 2] << 16) ^ (in[4 * (n) + 3] << 24) ^ ctx->w[m]

#define OUTUNPACK(n, x, m) \
   x ^= ctx->w[m]; \
   out[4 * (n)] = x; out[4 * (n) + 1] = x >> 8; \
   out[4 * (n) + 2] = x >> 16; out[4 * (n) + 3] = x >> 24


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

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* TWOFISH_H_ */
