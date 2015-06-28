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
 * @file        chacha.c
 * @brief       implementation of the ChaCha stream cipher 
 *
 * @author      Jan Wagner <mail@jwagner.eu>
 * @author      D. J. Bernstein 
 *
 * @}
 */

#include <stdio.h>
#include <stdint.h>

#include "crypto/chacha.h"

/* Perform rounds/2 number of doublerounds */
void doublerounds(uint8_t output[64], const uint32_t input[16], uint8_t rounds)
{
  uint32_t x[16];
  int32_t i;

  for (i = 0;i < 16;++i) { 
    x[i] = input[i];
  }

  for (i = rounds ; i > 0 ; i -= 2) {
    QUARTERROUND( 0, 4, 8,12)
    QUARTERROUND( 1, 5, 9,13)
    QUARTERROUND( 2, 6,10,14)
    QUARTERROUND( 3, 7,11,15)

    QUARTERROUND( 0, 5,10,15)
    QUARTERROUND( 1, 6,11,12)
    QUARTERROUND( 2, 7, 8,13)
    QUARTERROUND( 3, 4, 9,14)
  }

  for (i = 0;i < 16;++i) {
    x[i] = PLUS(x[i], input[i]);
  }

  for (i = 0;i < 16;++i) {
    U32TO8_LITTLE(output + 4 * i, x[i]);
  }
}


/* Initializes the given cipher context with key, iv and constants */
void init(chacha_ctx *x, uint8_t *key, uint32_t keylen, uint8_t *iv)
{
  if (keylen == 256) {
    // 256 bit key.
    x->state[0]  = SIGMA[0] | (SIGMA[1] << 8) | (SIGMA[2] << 16) | (SIGMA[3] << 24);
    /* fix to bypass: type-punned pointer strict-aliasing warning */ 

    x->state[1]  = U8TO32_LITTLE(SIGMA + 4);
    x->state[2]  = U8TO32_LITTLE(SIGMA + 8);
    x->state[3]  = U8TO32_LITTLE(SIGMA + 12);
    x->state[4]  = U8TO32_LITTLE(key + 0);
    x->state[5]  = U8TO32_LITTLE(key + 4);
    x->state[6]  = U8TO32_LITTLE(key + 8);
    x->state[7]  = U8TO32_LITTLE(key + 12);
    x->state[8]  = U8TO32_LITTLE(key + 16);
    x->state[9]  = U8TO32_LITTLE(key + 20);
    x->state[10] = U8TO32_LITTLE(key + 24);
    x->state[11] = U8TO32_LITTLE(key + 28);
  } 

  else {
    // 128 bit key.
    x->state[0]  = TAU[0] | (TAU[1] << 8) | (TAU[2] << 16) | (TAU[3] << 24);
    /* fix to bypass: type-punned pointer strict-aliasing warning */ 

    x->state[1]  = U8TO32_LITTLE(TAU + 4);
    x->state[2]  = U8TO32_LITTLE(TAU + 8);
    x->state[3]  = U8TO32_LITTLE(TAU + 12);
    x->state[4]  = U8TO32_LITTLE(key + 0);
    x->state[5]  = U8TO32_LITTLE(key + 4);
    x->state[6]  = U8TO32_LITTLE(key + 8);
    x->state[7]  = U8TO32_LITTLE(key + 12);
    x->state[8]  = U8TO32_LITTLE(key + 0);
    x->state[9]  = U8TO32_LITTLE(key + 4);
    x->state[10] = U8TO32_LITTLE(key + 8);
    x->state[11] = U8TO32_LITTLE(key + 12);
  }
    
  // Reset block counter and add IV to state.
  x->state[12] = 0;
  x->state[13] = 0;
  x->state[14] = U8TO32_LITTLE(iv + 0);
  x->state[15] = U8TO32_LITTLE(iv + 4);
}


/* tranform (encrypt/decrypt) the (next) block */
void next(chacha_ctx *ctx, const uint8_t *m, uint8_t *c)
{
  // Temporary internal state x.
  uint8_t x[64];
  uint8_t i;

  
  // Update the internal state and increase the block counter.
  doublerounds(x, ctx->state, ctx->rounds);
  ctx->state[12] = PLUSONE(ctx->state[12]);
  if (!ctx->state[12]) {
    ctx->state[13] = PLUSONE(ctx->state[13]);
  }

  // XOR the input block with the new temporal state to
  // create the transformed block.
  for (i = 0 ; i < 64 ; ++i) { 
    c[i] = m[i] ^ x[i];
  }
}


/* init a given ChaCha context by setting state to zero */
void init_ctx(chacha_ctx *ctx, uint8_t rounds)
{
  uint8_t i;

  for (i = 0 ; i < 16 ; i++) {
    ctx->state[i] = 0;
  }

  ctx->rounds = rounds;
}
