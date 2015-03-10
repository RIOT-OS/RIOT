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
 * @file        rabbit.c
 * @brief       Implementation of the Rabbit stream cipher
 *
 * @author      Jan Wagner <mail@jwagner.eu>
 * @author      Martin Boesgaard
 * @author      Mette Vesterager
 * @author      Thomas Pedersen
 * @author      Jesper Christensen
 * @author      Ove Scavenius
 *
 */

#include <stdio.h>
#include <string.h>
#include <stdint.h>

#include "crypto/rabbit.h"

/* Calculate the next internal state */
static void rabbit_next_state(rabbit_ctx *ctx)
{
    uint32_t g[8], c_old[8];

    c_old[0] = ctx->c[0];
    c_old[1] = ctx->c[1];
    c_old[2] = ctx->c[2];
    c_old[3] = ctx->c[3];
    c_old[4] = ctx->c[4];
    c_old[5] = ctx->c[5];
    c_old[6] = ctx->c[6];
    c_old[7] = ctx->c[7];

    /* Calculate new counter values */
    ctx->c[0] += 0x4D34D34D + ctx->carry;
    ctx->c[1] += 0xD34D34D3 + (ctx->c[0] < c_old[0]);
    ctx->c[2] += 0x34D34D34 + (ctx->c[1] < c_old[1]);
    ctx->c[3] += 0x4D34D34D + (ctx->c[2] < c_old[2]);
    ctx->c[4] += 0xD34D34D3 + (ctx->c[3] < c_old[3]);
    ctx->c[5] += 0x34D34D34 + (ctx->c[4] < c_old[4]);
    ctx->c[6] += 0x4D34D34D + (ctx->c[5] < c_old[5]);
    ctx->c[7] += 0xD34D34D3 + (ctx->c[6] < c_old[6]);
    ctx->carry = (ctx->c[7] < c_old[7]);

    /* Calculate the g-functions */
    g[0] = G_FUNC((ctx->x[0] + ctx->c[0]));
    g[1] = G_FUNC((ctx->x[1] + ctx->c[1]));
    g[2] = G_FUNC((ctx->x[2] + ctx->c[2]));
    g[3] = G_FUNC((ctx->x[3] + ctx->c[3]));
    g[4] = G_FUNC((ctx->x[4] + ctx->c[4]));
    g[5] = G_FUNC((ctx->x[5] + ctx->c[5]));
    g[6] = G_FUNC((ctx->x[6] + ctx->c[6]));
    g[7] = G_FUNC((ctx->x[7] + ctx->c[7]));

    /* Calculate new state values */
    ctx->x[0] = g[0] + ROTL32(g[7], 16) + ROTL32(g[6], 16);
    ctx->x[1] = g[1] + ROTL32(g[0], 8) + g[7];
    ctx->x[2] = g[2] + ROTL32(g[1], 16) + ROTL32(g[0], 16);
    ctx->x[3] = g[3] + ROTL32(g[2], 8) + g[1];
    ctx->x[4] = g[4] + ROTL32(g[3], 16) + ROTL32(g[2], 16);
    ctx->x[5] = g[5] + ROTL32(g[4], 8) + g[3];
    ctx->x[6] = g[6] + ROTL32(g[5], 16) + ROTL32(g[4], 16);
    ctx->x[7] = g[7] + ROTL32(g[6], 8) + g[5];
}

/* Rabbit Key setup */
void rabbit_keysetup(rabbit_ctx *ctx, const uint8_t *key)
{
    uint32_t k0, k1, k2, k3;

    /* Generate four subkeys */
    k0 = *(uint32_t *)(key + 0);
    k1 = *(uint32_t *)(key + 4);
    k2 = *(uint32_t *)(key + 8);
    k3 = *(uint32_t *)(key + 12);

    /* Generate initial state variables */
    ctx->x[0] = k0;
    ctx->x[2] = k1;
    ctx->x[4] = k2;
    ctx->x[6] = k3;
    ctx->x[1] = (k3 << 16) | (k2 >> 16);
    ctx->x[3] = (k0 << 16) | (k3 >> 16);
    ctx->x[5] = (k1 << 16) | (k0 >> 16);
    ctx->x[7] = (k2 << 16) | (k1 >> 16);

    /* Generate initial counter values */
    ctx->c[0] = ROTL32(k2, 16);
    ctx->c[2] = ROTL32(k3, 16);
    ctx->c[4] = ROTL32(k0, 16);
    ctx->c[6] = ROTL32(k1, 16);
    ctx->c[1] = (k0 & 0xFFFF0000) | (k1 & 0xFFFF);
    ctx->c[3] = (k1 & 0xFFFF0000) | (k2 & 0xFFFF);
    ctx->c[5] = (k2 & 0xFFFF0000) | (k3 & 0xFFFF);
    ctx->c[7] = (k3 & 0xFFFF0000) | (k0 & 0xFFFF);

    ctx->carry = 0;

    /* Iterate the system four times */
    rabbit_next_state(ctx);
    rabbit_next_state(ctx);
    rabbit_next_state(ctx);
    rabbit_next_state(ctx);

    /* Modify the counters */
    ctx->c[0] ^= ctx->x[(0 + 4) & 0x7];
    ctx->c[1] ^= ctx->x[(1 + 4) & 0x7];
    ctx->c[2] ^= ctx->x[(2 + 4) & 0x7];
    ctx->c[3] ^= ctx->x[(3 + 4) & 0x7];
    ctx->c[4] ^= ctx->x[(4 + 4) & 0x7];
    ctx->c[5] ^= ctx->x[(5 + 4) & 0x7];
    ctx->c[6] ^= ctx->x[(6 + 4) & 0x7];
    ctx->c[7] ^= ctx->x[(7 + 4) & 0x7];
}

/* Rabbit IV setup */
void rabbit_ivsetup(const rabbit_ctx *master_ctx, rabbit_ctx *ctx, const uint8_t *iv)
{
    uint32_t i0, i1, i2, i3;

    /* Generate four subvectors */
    i0 = *(uint32_t *)(iv + 0);
    i2 = *(uint32_t *)(iv + 4);
    i1 = (i0 >> 16) | (i2 & 0xFFFF0000);
    i3 = (i2 << 16) | (i0 & 0x0000FFFF);

    /* Modify counter values */
    ctx->c[0] = master_ctx->c[0] ^ i0;
    ctx->c[1] = master_ctx->c[1] ^ i1;
    ctx->c[2] = master_ctx->c[2] ^ i2;
    ctx->c[3] = master_ctx->c[3] ^ i3;
    ctx->c[4] = master_ctx->c[4] ^ i0;
    ctx->c[5] = master_ctx->c[5] ^ i1;
    ctx->c[6] = master_ctx->c[6] ^ i2;
    ctx->c[7] = master_ctx->c[7] ^ i3;

    /* Copy internal state values */
    ctx->x[0] = master_ctx->x[0];
    ctx->x[1] = master_ctx->x[1];
    ctx->x[2] = master_ctx->x[2];
    ctx->x[3] = master_ctx->x[3];
    ctx->x[4] = master_ctx->x[4];
    ctx->x[5] = master_ctx->x[5];
    ctx->x[6] = master_ctx->x[6];
    ctx->x[7] = master_ctx->x[7];
    ctx->carry = master_ctx->carry;

    /* Iterate the system four times */
    rabbit_next_state(ctx);
    rabbit_next_state(ctx);
    rabbit_next_state(ctx);
    rabbit_next_state(ctx);
}

/* Encrypt or decrypt "msglen" number of message bytes */
void rabbit_process_bytes(rabbit_ctx *ctx, const uint8_t *input, uint8_t *output, uint32_t msglen)
{
    uint32_t i;

    for (i = 0; i < msglen; i += 16) {
        /* Iterate the system */
        rabbit_next_state(ctx);

        /* Encrypt 16 bytes of data */
        *(uint32_t *)(output + 0) = 
	   *(uint32_t *)(input + 0) ^ ctx->x[0] ^ (ctx->x[5] >> 16) ^ (ctx->x[3] << 16);

        *(uint32_t *)(output + 4) =
	   *(uint32_t *)(input + 4) ^ ctx->x[2] ^ (ctx->x[7] >> 16) ^ (ctx->x[5] << 16);

        *(uint32_t *)(output + 8) =
	   *(uint32_t *)(input + 8) ^ ctx->x[4] ^ (ctx->x[1] >> 16) ^ (ctx->x[7] << 16);

        *(uint32_t *)(output + 12) =
	   *(uint32_t *)(input + 12) ^ ctx->x[6] ^ (ctx->x[3] >> 16) ^ (ctx->x[1] << 16);

        input += 16;
        output += 16;
    }
}

/* Generate data with Pseudo-Random Number Generator */
void rabbit_prng(rabbit_ctx *ctx, uint8_t *output, uint32_t msglen)
{
    uint32_t i;

    for (i = 0; i < msglen; i += 16) {
        /* Iterate the system */
        rabbit_next_state(ctx);

        /* Generate 16 bytes of pseudo-random data */
        *(uint32_t *)(output + 0) = ctx->x[0] ^ (ctx->x[5] >> 16) ^ (ctx->x[3] << 16);
        *(uint32_t *)(output + 4) = ctx->x[2] ^ (ctx->x[7] >> 16) ^ (ctx->x[5] << 16);
        *(uint32_t *)(output + 8) = ctx->x[4] ^ (ctx->x[1] >> 16) ^ (ctx->x[7] << 16);
        *(uint32_t *)(output + 12) = ctx->x[6] ^ (ctx->x[3] >> 16) ^ (ctx->x[1] << 16);

        output += 16;
    }
}
/** @} */
