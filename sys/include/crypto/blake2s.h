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
 * @brief       Headers for the implementation of the BLAKE2s hash function
 *
 * @author      Jan Wagner <mail@jwagner.eu>
 * @author      Jean-Philippe Aumasson
 * @author      Samuel Neves
 * @author      Zooko Wilcox-O'Hearn
 * @author      Christian Winnerlein
 *
 */

#ifndef __BLAKE2S_H_
#define __BLAKE2S_H_

#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Algorithm constants
 * @{
 */
#define BLAKE2S_BLOCKBYTES	64
#define BLAKE2S_OUTBYTES	32
#define BLAKE2S_KEYBYTES	32
#define BLAKE2S_SALTBYTES	8
#define BLAKE2S_PERSONALBYTES	8

#define BLAKE2B_SALTBYTES	16
/** @} */

/**
 * @brief BLAKE2s parameter collection
 */
typedef struct {
    uint8_t  digest_length; // 1
    uint8_t  key_length;    // 2
    uint8_t  fanout;        // 3
    uint8_t  depth;         // 4
    uint32_t leaf_length;   // 8
    uint8_t  node_offset[6];// 14
    uint8_t  node_depth;    // 15
    uint8_t  inner_length;  // 16
    // uint8_t  reserved[0];
    uint8_t  salt[BLAKE2B_SALTBYTES]; // 24
    uint8_t  personal[BLAKE2S_PERSONALBYTES];  // 32
} blake2s_param;

/**
 * @brief BLAKE2s state context
 */
typedef struct {
    uint32_t h[8];
    uint32_t t[2];
    uint32_t f[2];
    uint8_t  buf[2 * BLAKE2S_BLOCKBYTES];
    size_t   buflen;
    uint8_t  last_node;
} blake2s_state;

/**
 * @name Algorithm macros
 * @{
 */
#define G(r,i,a,b,c,d) \
  do { \
    a = a + b + m[blake2s_sigma[r][2*i+0]]; \
    d = rotr32(d ^ a, 16); \
    c = c + d; \
    b = rotr32(b ^ c, 12); \
    a = a + b + m[blake2s_sigma[r][2*i+1]]; \
    d = rotr32(d ^ a, 8); \
    c = c + d; \
    b = rotr32(b ^ c, 7); \
  } while (0)

#define BLAKE2S_ROUND(r)  \
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
 * @brief  Sequential BLAKE2s initialization
 *
 * @param[in]   S		BLAKE2s status context
 * @param[in]   outlen		Length of output digest
 *
 * @return 0 if successful, -1 on error
 */
int blake2s_init(blake2s_state *S, const uint8_t outlen);

/**
 * @brief  Initialization of BLAKE2s hash with key
 *
 * @param[in]   S		BLAKE2s status context
 * @param[in]   outlen		Length of output digest
 * @param[in]   key		Input key
 * @param[in]   keylen		Length of key
 *
 * @return 0 if successful, -1 on error
 */
int blake2s_init_key(blake2s_state *S, const uint8_t outlen, const void *key, const uint8_t keylen);

/**
 * @brief  Initialization by XORing BLAKE2s IVs with input parameter block
 *
 * @param[in]   S		BLAKE2s status context
 * @param[in]   P		BLAKE2s parameter input struct
 *
 * @return 0 if successful
 */
int blake2s_init_param(blake2s_state *S, const blake2s_param *P);

/**
 * @brief  Add bytes by performing BLAKE2s block compression
 *
 * @param[in]   S		BLAKE2s status context
 * @param[in]   in		Input bytes
 * @param[in]   inlen		Length of byte input
 *
 * @return 0 if successful
 */
int blake2s_update(blake2s_state *S, const uint8_t *in, uint64_t inlen);

/**
 * @brief  BLAKE2s finalization
 *
 * @param[in]   S		BLAKE2s status context
 * @param[out]  out		Variable for finalized output
 * @param[in]   outlen		Length of output variable
 *
 * @return 0 if successful
 */
int blake2s_final(blake2s_state *S, uint8_t *out, uint8_t outlen);

/**
 * @brief  Calculate BLAKE2s hash from input using key
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
int blake2s(uint8_t *out, const void *in, const void *key, const uint8_t outlen,
            const uint64_t inlen, uint8_t keylen);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* __BLAKE2S_H_ */
