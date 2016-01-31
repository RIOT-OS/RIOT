/*
 * Copyright (C) 2015 Jan Wagner <mail@jwagner.eu>
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
 * @brief       Headers for the implementation of the BLAKE2b hash function
 *
 * @author      Jan Wagner <mail@jwagner.eu>
 * @author      Jean-Philippe Aumasson
 * @author      Samuel Neves
 * @author      Zooko Wilcox-O'Hearn
 * @author      Christian Winnerlein
 *
 */

#ifndef __BLAKE2B_H_
#define __BLAKE2B_H_

#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Algorithm constants
 * @{
 */
#define BLAKE2B_BLOCKBYTES	128
#define BLAKE2B_OUTBYTES	64
#define BLAKE2B_KEYBYTES	64
#define BLAKE2B_SALTBYTES	16
#define BLAKE2B_PERSONALBYTES	16
/** @} */

/**
 * @brief BLAKE2b parameter collection
 */
typedef struct {
    uint8_t  digest_length; // 1
    uint8_t  key_length;    // 2
    uint8_t  fanout;        // 3
    uint8_t  depth;         // 4
    uint32_t leaf_length;   // 8
    uint64_t node_offset;   // 16
    uint8_t  node_depth;    // 17
    uint8_t  inner_length;  // 18
    uint8_t  reserved[14];  // 32
    uint8_t  salt[BLAKE2B_SALTBYTES]; // 48
    uint8_t  personal[BLAKE2B_PERSONALBYTES];  // 64
} blake2b_param;

/**
 * @brief BLAKE2b state context
 */
typedef struct {
    uint64_t h[8];
    uint64_t t[2];
    uint64_t f[2];
    uint8_t  buf[2 * BLAKE2B_BLOCKBYTES];
    size_t   buflen;
    uint8_t  last_node;
} blake2b_state;

/**
 * @name Algorithm macros
 * @{
 */
#define G(r,i,a,b,c,d) \
   do { \
      a = a + b + m[blake2b_sigma[r][2*i+0]]; \
      d = rotr64(d ^ a, 32); \
      c = c + d; \
      b = rotr64(b ^ c, 24); \
      a = a + b + m[blake2b_sigma[r][2*i+1]]; \
      d = rotr64(d ^ a, 16); \
      c = c + d; \
      b = rotr64(b ^ c, 63); \
   } while (0)

#define BLAKE2B_ROUND(r)  \
   do { \
      G(r,0,v[ 0],v[ 4],v[ 8],v[12]); \
      G(r,1,v[ 1],v[ 5],v[ 9],v[13]); \
      G(r,2,v[ 2],v[ 6],v[10],v[14]); \
      G(r,3,v[ 3],v[ 7],v[11],v[15]); \
      G(r,4,v[ 0],v[ 5],v[10],v[15]); \
      G(r,5,v[ 1],v[ 6],v[11],v[12]); \
      G(r,6,v[ 2],v[ 7],v[ 8],v[13]); \
      G(r,7,v[ 3],v[ 4],v[ 9],v[14]); \
   } while (0)
/** @} */

/**
 * @brief  Sequential BLAKE2b initialization
 *
 * @param[in]   S		BLAKE2b status context
 * @param[in]   outlen		Length of output digest
 *
 * @return 0 if successful, -1 on error
 */
int blake2b_init(blake2b_state *S, const uint8_t outlen);

/**
 * @brief  Initialization of BLAKE2b hash with key
 *
 * @param[in]   S		BLAKE2b status context
 * @param[in]   outlen		Length of output digest
 * @param[in]   key		Input key
 * @param[in]   keylen		Length of key
 *
 * @return 0 if successful, -1 on error
 */
int blake2b_init_key(blake2b_state *S, const uint8_t outlen, const void *key, const uint8_t keylen);

/**
 * @brief  Initialization by XORing BLAKE2b IVs with input parameter block
 *
 * @param[in]   S		BLAKE2b status context
 * @param[in]   P		BLAKE2b parameter input struct
 *
 * @return 0 if successful
 */
int blake2b_init_param(blake2b_state *S, const blake2b_param *P);

/**
 * @brief  Add bytes by performing BLAKE2b block compression
 *
 * @param[in]   S		BLAKE2b status context
 * @param[in]   in		Input bytes
 * @param[in]   inlen		Length of byte input
 *
 * @return 0 if successful
 */
int blake2b_update(blake2b_state *S, const uint8_t *in, uint64_t inlen);

/**
 * @brief  BLAKE2b finalization
 *
 * @param[in]   S		BLAKE2b status context
 * @param[out]  out		Variable for finalized output
 * @param[in]   outlen		Length of output variable
 *
 * @return 0 if successful
 */
int blake2b_final(blake2b_state *S, uint8_t *out, uint8_t outlen);

/**
 * @brief  Calculate BLAKE2b hash from input using key
 *
 * @param[out]  out		Hash digest output variable
 * @param[in]   in		Input bytes to hash
 * @param[in]   key		Input key to use
 * @param[in]   outlen		Length of key
 * @param[in]   inlen		Length of input bytes
 * @param[in]   keylen		Length of key
 *
 * @return 0 if successful, -1 on error
 */
int blake2b(uint8_t *out, const void *in, const void *key, const uint8_t outlen,
            const uint64_t inlen, uint8_t keylen);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* __BLAKE2S_B_ */
