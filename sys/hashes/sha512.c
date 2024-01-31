/*
 * Copyright (C) 2023 TU Dresden
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_hashes
 * @{
 *
 * @file
 * @brief       SHA-512 hash function implementation
 *
 * @author      Mikolai Gütschow <mikolai.guetschow@tu-dresden.de>
 *
 * @}
 */

#include <string.h>
#include <assert.h>

#include "hashes/sha512.h"
#include "hashes/sha512_common.h"

/* SHA-512 initialization.  Begins a SHA-512 operation. */
void sha512_init(sha512_context_t *ctx)
{
    /* Zero bits processed so far */
    ctx->count[0] = ctx->count[1] = 0;

    /* Magic initialization constants */
    ctx->state[0] = 0x6a09e667f3bcc908;
    ctx->state[1] = 0xbb67ae8584caa73b;
    ctx->state[2] = 0x3c6ef372fe94f82b;
    ctx->state[3] = 0xa54ff53a5f1d36f1;
    ctx->state[4] = 0x510e527fade682d1;
    ctx->state[5] = 0x9b05688c2b3e6c1f;
    ctx->state[6] = 0x1f83d9abfb41bd6b;
    ctx->state[7] = 0x5be0cd19137e2179;
}

void sha512(const void *data, size_t len, void *digest)
{
    sha512_context_t c;

    sha512_init(&c);
    sha512_update(&c, data, len);
    sha512_final(&c, digest);
}
