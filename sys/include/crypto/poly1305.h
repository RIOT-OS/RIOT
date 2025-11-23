/*
 * SPDX-FileCopyrightText: 2016 Andrew Moon (dedicated to the public domain)
 * SPDX-FileCopyrightText: 2018 Freie Universität Berlin
 * SPDX-FileCopyrightText: 2018 Inria
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     sys_crypto
 * @defgroup    sys_crypto_poly1305 poly1305
 * @brief       Poly1305 one-time message authentication code
 *
 * Poly1305 is a one-time authenticator designed by D.J. Bernstein. It uses a
 * 32-byte one-time key and a message and produces a 16-byte tag.
 *
 * @{
 *
 * @file
 * @brief       Poly1305 MAC interface
 *
 * @author      Koen Zandberg <koen@bergzand.net>
 *
 * @see         https://tools.ietf.org/html/rfc8439#section-2.5
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <stddef.h>
#include <stdint.h>

/**
 * @brief Poly1305 block size
 */
#define POLY1305_BLOCK_SIZE 16

/**
 * @brief Poly1305 context
 */
typedef struct {
    uint32_t r[4];                          /**< first key part         */
    uint32_t pad[4];                        /**< Second key part        */
    uint32_t h[5];                          /**< Hash                   */
    uint32_t c[4];                          /**< Message chunk          */
    size_t c_idx;                           /**< Chunk length            */
} poly1305_ctx_t;

/**
 * @brief   Initialize a poly1305 context
 *
 * @param   ctx     Poly1305 context
 * @param   key     32 byte key
 */
void poly1305_init(poly1305_ctx_t *ctx, const uint8_t *key);

/**
 * @brief   Update the poly1305 context with a block of message
 *
 * @param   ctx     poly1305 context
 * @param   data    ptr to the message
 * @param   len     length of the message
 */
void poly1305_update(poly1305_ctx_t *ctx, const uint8_t *data, size_t len);

/**
 * @brief   Finish the poly1305 operation
 *
 * @param   ctx     poly1305 context
 * @param   mac     16 byte buffer for the tag
 */
void poly1305_finish(poly1305_ctx_t *ctx, uint8_t *mac);

/**
 * @brief   Calculate a single poly1305 tag
 *
 * @param   mac     16 byte buffer for the tag
 * @param   data    ptr to the message
 * @param   len     length of the message
 * @param   key     32 byte key
 */
void poly1305_auth(uint8_t *mac, const uint8_t *data, size_t len,
                   const uint8_t *key);

#ifdef __cplusplus
}
#endif
/** @} */
