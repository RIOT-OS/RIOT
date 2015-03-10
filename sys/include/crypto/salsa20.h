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
 * @file        salsa20.h
 * @brief       Headers for the implementation of the Salsa20 stream cipher
 *
 * @author      Jan Wagner <mail@jwagner.eu>
 * @author      D. J. Bernstein
 *
 */

#ifndef __SALSA20_H_
#define __SALSA20_H_

#include <inttypes.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief The macro definitions
 */
#define XOR(v,w) ((v) ^ (w))
#define PLUS(v,w) ((uint32_t)((v) + (w)))
#define PLUSONE(v) (PLUS((v),1))

#define ROTL32(v,c) ((uint32_t)((v) << (c)) | ((v) >> (32 - (c))))
#define U8TO32_LITTLE(p) (((uint32_t*)(p))[0])
#define U32TO8_LITTLE(p, v) (((uint32_t*)(p))[0] = v)

/**
 * @brief The Salsa20 state context
 */
typedef struct {
    uint32_t input[16]; /* could be compressed */
} salsa20_ctx;

/**
 * @brief  Salsa20 Key setup
 *
 * @param[in]   ctx         Salsa20 context
 * @param[in]   key         Key to setup the ctx
 * @param[in]   keysize     Size of the key in bits
 */
void salsa20_keysetup(salsa20_ctx *ctx, const uint8_t *key, uint32_t keysize);

/**
 * @brief  Salsa20 IV setup
 *
 * @param[in]   ctx         Salsa20 context
 * @param[in]   iv          IV to setup the ctx
 */
void salsa20_ivsetup(salsa20_ctx *ctx, const uint8_t *iv);

/**
 * @brief  Encrypt or decrypt "msglen" number of message bytes
 *
 * @param[in]   ctx         Salsa20 context
 * @param[in]   input       (Plain/Cipher)text for (en/de)crypting
 * @param[out]  output      (Cipher/Plain)text for (en/de)crypting
 * @param[in]   msglen      Number of bytes to (en/de)crypt
 */
void salsa20_process_bytes(salsa20_ctx *ctx, const uint8_t *input, uint8_t *output,
                           uint32_t msglen);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* __SALSA20_H_ */
