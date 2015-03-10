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
 * @file        rabbit.h
 * @brief       Headers for the implementation of the Rabbit stream cipher
 *
 * @author      Jan Wagner <mail@jwagner.eu>
 * @author      Martin Boesgaard
 * @author      Mette Vesterager
 * @author      Thomas Pedersen
 * @author      Jesper Christensen
 * @author      Ove Scavenius
 *
 */

#ifndef __RABBIT_H_
#define __RABBIT_H_

#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief The macro definitions
 */
#define ROTL32(v, n) ((uint32_t)(((v) << (n)) & 0xFFFFFFFF) | ((v) >> (32 - (n))))

#define G_FUNC(x) \
((((((x & 0xFFFF) * (x & 0xFFFF))>>17) + ((x & 0xFFFF)*(x >> 16)))>>15) \
+ (x >> 16)*(x >> 16)) ^ (x * x)

/**
 * @brief The Rabbit state context
 */
typedef struct {
    uint32_t x[8];
    uint32_t c[8];
    uint32_t carry;
} rabbit_ctx;

/**
 * @brief  Rabbit Key setup
 *
 * @param[in]   ctx     Rabbit context
 * @param[in]   key     Key to setup the ctx
 */
void rabbit_keysetup(rabbit_ctx *ctx, const uint8_t *key);

/**
 * @brief  Rabbit IV setup
 *
 * @param[in]   ctx     Rabbit context
 * @param[in]   iv      IV to setup the ctx
 */
void rabbit_ivsetup(const rabbit_ctx *master_ctx, rabbit_ctx *ctx, const uint8_t *iv);

/**
 * @brief  Encrypt or decrypt "msglen" number of message bytes
 *
 * @param[in]   ctx         Rabbit context
 * @param[in]   input       (Plain/Cipher)text for (en/de)crypting
 * @param[out]  output      (Cipher/Plain)text for (en/de)crypting
 * @param[in]   msglen      Number of bytes to (en/de)crypt
 */
void rabbit_process_bytes(rabbit_ctx *ctx, const uint8_t *input, uint8_t *output, uint32_t msglen);

/**
 * @brief  Generate data with Pseudo-Random Number Generator
 *
 * @param[in]   ctx         Rabbit context
 * @param[out]  output      Generated ciphertext
 * @param[in]   msglen      Number of bytes to output
 */
void rabbit_prng(rabbit_ctx *ctx, uint8_t *output, uint32_t msglen);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* __RABBIT_H_ */
