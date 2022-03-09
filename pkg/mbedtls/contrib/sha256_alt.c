/*
 * Copyright (C) 2020 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_mbedtls
 *
 * @{
 * @file
 *
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 *
 * @}
 */

#include <assert.h>
#include <string.h>

#include "sha256_alt.h"

#if IS_ACTIVE(MBEDTLS_SHA256_ALT)
#include "mbedtls/sha256.h"
#include "mbedtls/platform.h"

void mbedtls_sha256_init(mbedtls_sha256_context *ctx)
{
    assert(ctx != NULL);

    memset(ctx, 0, sizeof(mbedtls_sha256_context));
}

void mbedtls_sha256_free(mbedtls_sha256_context *ctx)
{
    if (ctx == NULL ) {
        return;
    }
    memset(ctx, 0, sizeof(mbedtls_sha256_context));
}

void mbedtls_sha256_clone(mbedtls_sha256_context *dst,
                           const mbedtls_sha256_context *src)
{
    if (dst == NULL || src == NULL) {
        return;
    }
    memcpy(dst, src, sizeof(mbedtls_sha256_context));
}

int mbedtls_sha256_starts_ret(mbedtls_sha256_context *ctx, int is224)
{
    assert(ctx != NULL);

    ctx->is224 = is224;
    if (is224) {
        sha224_init(&ctx->riot_sha256_ctx);
    }
    else {
        sha256_init(&ctx->riot_sha256_ctx);
    }
    return 0;
}

int mbedtls_internal_sha256_process(mbedtls_sha256_context *ctx,
                                     const unsigned char data[64])
{
    (void)ctx;
    (void)data;

    return MBEDTLS_ERR_PLATFORM_FEATURE_UNSUPPORTED;
}

int mbedtls_sha256_update_ret(mbedtls_sha256_context *ctx,
                               const unsigned char *input,
                               size_t ilen)
{
    assert(ctx != NULL && input != NULL);

    sha256_update(&ctx->riot_sha256_ctx, input, ilen);
    return 0;
}

int mbedtls_sha256_finish_ret(mbedtls_sha256_context *ctx,
                               unsigned char output[32])
{
    assert(ctx != NULL && output != NULL);

    if (ctx->is224) {
        sha224_final(&ctx->riot_sha256_ctx, output);
    }
    else {
        sha256_final(&ctx->riot_sha256_ctx, output);
    }
    return 0;
}
#endif /* MBEDTLS_SHA256_ALT */
