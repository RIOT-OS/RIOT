/*-
 * Copyright 2014 Kenneth MacKay
 * Copyright 2014 Frank Holtz
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
 * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
 * ARE DISCLAIMED.  IN NO EVENT SHALL THE AUTHOR OR CONTRIBUTORS BE LIABLE
 * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
 * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
 * OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION)
 * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
 * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
 * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
 * SUCH DAMAGE.
 *
 */

/**
 * @file
 * @brief       Check if the micro-ecc builds and working
 *
 * @author      Frank Holtz <frank-riot2015@holtznet.de>
 *
 */

#include <stdio.h>
#include <string.h>
#include "test_utils/expect.h"
#include "hashes/sha256.h"
#include "uECC.h"

#define TESTROUNDS              (4)
#define MAX_PUBLIC_KEY_SIZE     (64)
#define MAX_CURVE_SIZE          (32)

typedef struct uECC_SHA256_HashContext {
    uECC_HashContext uECC;
    sha256_context_t ctx;
} uECC_SHA256_HashContext;

static uint8_t l_secret1[MAX_CURVE_SIZE];
static uint8_t l_secret2[MAX_CURVE_SIZE];
static uint8_t l_sig[MAX_PUBLIC_KEY_SIZE];
/* use pre-generated keys for no-HWRNG platforms */
static const uint8_t l_private1[] = {
    0x9b, 0x4c, 0x4b, 0xa0, 0xb7, 0xb1, 0x25, 0x23,
    0x9c, 0x09, 0x85, 0x4f, 0x9a, 0x21, 0xb4, 0x14,
    0x70, 0xe0, 0xce, 0x21, 0x25, 0x00, 0xa5, 0x62,
    0x34, 0xa4, 0x25, 0xf0, 0x0f, 0x00, 0xeb, 0xe7,
};
static const uint8_t l_public1[] = {
    0x54, 0x3e, 0x98, 0xf8, 0x14, 0x55, 0x08, 0x13,
    0xb5, 0x1a, 0x1d, 0x02, 0x02, 0xd7, 0x0e, 0xab,
    0xa0, 0x98, 0x74, 0x61, 0x91, 0x12, 0x3d, 0x96,
    0x50, 0xfa, 0xd5, 0x94, 0xa2, 0x86, 0xa8, 0xb0,
    0xd0, 0x7b, 0xda, 0x36, 0xba, 0x8e, 0xd3, 0x9a,
    0xa0, 0x16, 0x11, 0x0e, 0x1b, 0x6e, 0x81, 0x13,
    0xd7, 0xf4, 0x23, 0xa1, 0xb2, 0x9b, 0xaf, 0xf6,
    0x6b, 0xc4, 0x2a, 0xdf, 0xbd, 0xe4, 0x61, 0x5c,
};
static const uint8_t l_private2[] = {
    0xb5, 0x45, 0xaf, 0xa0, 0x2e, 0x5c, 0xa6, 0x17,
    0x3b, 0x5a, 0x55, 0x76, 0x67, 0x5d, 0xd4, 0x5e,
    0x41, 0x7c, 0x4f, 0x19, 0x9f, 0xb9, 0x75, 0xdc,
    0xba, 0x57, 0xc4, 0xa2, 0x26, 0xc6, 0x86, 0x2a,
};
static const uint8_t l_public2[] = {
    0x2e, 0x81, 0x24, 0x3c, 0x44, 0xac, 0x63, 0x13,
    0x9b, 0xc1, 0x27, 0xe9, 0x53, 0x3b, 0x0a, 0xe2,
    0xf9, 0x22, 0xcd, 0x06, 0xfd, 0x12, 0x17, 0x2e,
    0xe5, 0x0e, 0xb5, 0xce, 0x6b, 0x50, 0xe2, 0x44,
    0xbf, 0x6b, 0x3f, 0xe8, 0x4e, 0x70, 0xd1, 0x06,
    0x85, 0x84, 0xb8, 0xef, 0xe2, 0x25, 0x91, 0x21,
    0xf3, 0x46, 0x70, 0xa9, 0x1c, 0x79, 0x19, 0xe3,
    0xfb, 0x11, 0x36, 0x64, 0x37, 0x64, 0x58, 0xc9,
};
static uint8_t tmp[2 * SHA256_DIGEST_LENGTH + SHA256_INTERNAL_BLOCK_SIZE];

/* reserve space for a SHA-256 hash */
static uint8_t l_hash[32] = { 0 };

static void _init_sha256(const uECC_HashContext *base)
{
    uECC_SHA256_HashContext *context = (uECC_SHA256_HashContext*)base;
    sha256_init(&context->ctx);
}

static void _update_sha256(const uECC_HashContext *base,
                          const uint8_t *message,
                          unsigned message_size)
{
    uECC_SHA256_HashContext *context = (uECC_SHA256_HashContext*)base;
    sha256_update(&context->ctx, message, message_size);
}

static void _finish_sha256(const uECC_HashContext *base, uint8_t *hash_result)
{
    uECC_SHA256_HashContext *context = (uECC_SHA256_HashContext*)base;
    sha256_final(&context->ctx, hash_result);
}

int main(void)
{
    printf("micro-ecc compiled!\n");

    const struct uECC_Curve_t *curve = uECC_secp256r1();
    int errorc = 0;

    expect(uECC_curve_private_key_size(curve) <= MAX_CURVE_SIZE);
    expect(uECC_curve_public_key_size(curve) <= MAX_PUBLIC_KEY_SIZE);

    printf("Testing %d random private key pairs and signature without using HWRNG\n", TESTROUNDS);

    for (int i = 0; i < TESTROUNDS; ++i) {
        printf("Round %d\n", i);

        if (!uECC_shared_secret(l_public2, l_private1, l_secret1, curve)) {
            printf("\nRound %d: shared_secret() failed (1)", i);
            errorc++;
        } else {
            if (!uECC_shared_secret(l_public1, l_private2, l_secret2, curve)) {
                printf("\nRound: %d: shared_secret() failed (2)", i);
                errorc++;
            } else {
                if (memcmp(l_secret1, l_secret2, sizeof(l_secret1)) != 0) {
                    printf("\nShared secrets are not identical!\n");
                    errorc++;
                }

                /* copy some bogus data into the hash */
                memcpy(l_hash, l_public1, 32);

                uECC_SHA256_HashContext ctx;
                ctx.uECC.init_hash = &_init_sha256;
                ctx.uECC.update_hash = &_update_sha256;
                ctx.uECC.finish_hash = &_finish_sha256;
                ctx.uECC.block_size = 64;
                ctx.uECC.result_size = 32;
                ctx.uECC.tmp = tmp;
                if (uECC_sign_deterministic(l_private1, l_hash, sizeof(l_hash),
                                            &ctx.uECC, l_sig, curve) != 1) {
                    printf("\nRound %d: uECC_sign_deterministic() failed", i);
                    errorc++;
                } else {
                    if (uECC_verify(l_public1, l_hash, sizeof(l_hash), l_sig,
                                    curve) != 1) {
                        printf("\nRound %d: uECC_verify() failed", i);
                        errorc++;
                    }
                }
            }
        }
    }

    printf("Done with %d error(s)\n", errorc);

    if (errorc) {
        puts("FAILURE");
        return 1;
    }
    else {
        puts("SUCCESS");
        return 0;
    }
}
