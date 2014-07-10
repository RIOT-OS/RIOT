/*
 * Copyright (C) 2014 Freie Universität Berlin, Computer Systems & Telematics
 *
 * This source code is licensed under the LGPLv2 license,
 * See the file LICENSE for more details.
 */

/**
 * @ingroup     sys_crypto
 * @{
 *
 * @file        null.h
 * @brief       null cipher.
 *
 * @author      Freie Universitaet Berlin, Computer Systems & Telematics
 * @author      Nico von Geyso <nico.geyso@fu-berlin.de>
 */

#include <stdint.h>
#include <string.h>
#include "crypto/ciphers.h"
#include "crypto/null.h"

// null interface
cipher_interface_t null_interface = {
    NULL_BLOCK_SIZE,
    NULL_KEY_SIZE,
    &null_init,
    &null_encrypt,
    &null_decrypt,
    &null_set_key
};


int null_init(cipher_context_t *context, uint8_t block_size, uint8_t *key,
        uint8_t key_size)
{
    null_context_t *ctx = (null_context_t*) context;

    if (context == NULL || null_set_key(context, key, key_size) < 0) {
        return -1;
    }

    if (block_size == 0) {
        return -1;
    }

    ctx->size = block_size;

    return 1;
}


int null_encrypt(cipher_context_t *context, uint8_t *in, uint8_t *out)
{
    null_context_t *ctx = (null_context_t*) context;

    if (context == NULL || in == NULL || out == NULL)
        return -1;

    memcpy(in, out, NULL_BLOCK_SIZE);

    return 1;
}


int null_set_key(cipher_context_t *context, uint8_t *key, uint8_t key_size)
{
    (void) context;

    if ( key_size > NULL_KEY_SIZE || key != NULL)
        return -1;

    return 1;
}
