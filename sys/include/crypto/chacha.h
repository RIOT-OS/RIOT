/*
 * Copyright (C) 2015 Freie Universität Berlin, Computer Systems & Telematics
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_crypto
 * @{
 *
 * @file        chacha.h
 * @brief       Headers for the implementation of the ChaCha stream cipher 
 *
 * @author      Jan Wagner <mail@jwagner.eu>
 * @author      D. J. Bernstein 
 *
 * @}
 */

#ifndef _CHACHA_H_
#define _CHACHA_H_

#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @brief The chacha state context 
 */
typedef struct
{
  uint32_t state[16];
  uint8_t rounds; 
} chacha_ctx;

/**
 * @brief The macro definitions
 */
#define ROTATE(v,c) ((uint32_t)((v) << (c)) | ((v) >> (32 - (c))))
#define XOR(v,w) ((v) ^ (w))
#define PLUS(v,w) ((uint32_t)((v) + (w)))
#define PLUSONE(v) (PLUS((v), 1))

#define U32TO8_LITTLE(p, v) (((uint32_t*)(p))[0] = v)
#define U8TO32_LITTLE(p) (((uint32_t*)(p))[0])

#define QUARTERROUND(a, b, c, d) \
  x[a] = PLUS(x[a],x[b]); x[d] = ROTATE(XOR(x[d],x[a]),16); \
  x[c] = PLUS(x[c],x[d]); x[b] = ROTATE(XOR(x[b],x[c]),12); \
  x[a] = PLUS(x[a],x[b]); x[d] = ROTATE(XOR(x[d],x[a]), 8); \
  x[c] = PLUS(x[c],x[d]); x[b] = ROTATE(XOR(x[b],x[c]), 7);

/**
 * @brief Storage arrays for expanded chacha keys 
 */
static const uint8_t SIGMA[16] = "expand 32-byte k";
static const uint8_t TAU[16]   = "expand 16-byte k";

/**
 * @brief   Init a given ChaCha context by setting state to zero  
 *
 * @param   ctx           the cipher context representing the internal cipher state
 * @param   rounds        the number of rounds 
 *
 * @return  void 
 */
void init_ctx(chacha_ctx *ctx, uint8_t rounds);

/**
 * @brief   Key and message independent initialization  
 *
 * @param   ctx           the cipher context representing the internal cipher state
 * @param   key           used ChaCha key
 * @param   keylen        length of the ChaCha key 
 * @param   iv            the initialisation vector 
 *
 * @return  void 
 */
void init(chacha_ctx *ctx, uint8_t *key, uint32_t keylen, uint8_t *iv);

/**
 * @brief   Given a pointer to the next block m of 64 cleartext bytes will use the given
 *          context to transform (encrypt/decrypt) the block 
 *
 * @param   ctx           the cipher context representing the internal cipher state
 * @param   m             a pointer to the next 64 cleartext bytes 
 * @param   c             a pointer used to store the transformated block 
 * @param   iv            the initialisation vector 
 *
 * @return  void 
 */
void next(chacha_ctx *ctx, const uint8_t *m, uint8_t *c);

/**
 * @brief   Perform rounds/2 number of doublerounds 
 *
 * @param   output        the output buffer used to store the result 
 * @param   input         internal state used as input 
 * @param   rounds        number of rounds 
 *
 * @return  void 
 */
void doublerounds(uint8_t output[64], const uint32_t input[16], uint8_t rounds);

#endif
