/*
 * Copyright (C) 2023 TU Dresden
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
 * @brief       SHA512/224 hash function implementation
 *
 * @author      Mikolai Gütschow <mikolai.guetschow@tu-dresden.de>
 *
 * @}
 */

#include <assert.h>

#include "hashes/sha512_224.h"

void sha512_224_init(sha512_224_context_t *ctx)
{
    /* Zero bits processed so far */
    ctx->count[0] = ctx->count[1] = 0;

    /* Magic initialization constants */
    ctx->state[0] = 0x8C3D37C819544DA2;
    ctx->state[1] = 0x73E1996689DCD4D6;
    ctx->state[2] = 0x1DFAB7AE32FF9C82;
    ctx->state[3] = 0x679DD514582F9FCF;
    ctx->state[4] = 0x0F6D2B697BD44DA8;
    ctx->state[5] = 0x77E36F7304C48942;
    ctx->state[6] = 0x3F9D85A86A1D36C8;
    ctx->state[7] = 0x1112E6AD91D692A1;
}

void sha512_224(const void *data, size_t len, void *digest)
{
    sha512_224_context_t c;
    assert(digest);

    sha512_224_init(&c);
    sha512_224_update(&c, data, len);
    sha512_224_final(&c, digest);
}
