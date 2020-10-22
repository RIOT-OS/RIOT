/*
 * Copyright (C) 2020 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_hashes
 *
 * @{
 * @file
 * @brief       SHA224 hash function implementation
 *
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 *
 * @}
 */

#include <string.h>

#include "hashes/sha224.h"
#include "hashes/sha2xx_common.h"

/* SHA-224 initialization.  Begins a SHA-224 operation. */
void sha224_init(sha224_context_t *ctx)
{
    /* Zero bits processed so far */
    ctx->count[0] = ctx->count[1] = 0;

    /* Magic initialization constants */
    ctx->state[0] = 0xC1059ED8;
    ctx->state[1] = 0x367CD507;
    ctx->state[2] = 0x3070DD17;
    ctx->state[3] = 0xF70E5939;
    ctx->state[4] = 0xFFC00B31;
    ctx->state[5] = 0x68581511;
    ctx->state[6] = 0x64F98FA7;
    ctx->state[7] = 0xBEFA4FA4;
}

void *sha224(const void *data, size_t len, void *digest)
{
    sha224_context_t c;
    static unsigned char m[SHA224_DIGEST_LENGTH];

    if (digest == NULL) {
        digest = m;
    }

    sha224_init(&c);
    sha224_update(&c, data, len);
    sha224_final(&c, digest);

    return digest;
}
