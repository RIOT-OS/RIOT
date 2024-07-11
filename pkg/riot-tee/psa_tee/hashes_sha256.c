/*
 * Copyright (C) 2024 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_riot_tee
 * @{
 *
 * @brief       Glue code translating between PSA Crypto and the RIOT TEE SHA256 APIs
 *
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 * @}
 */

#include "kernel_defines.h"
#include "psa/crypto.h"
#include "psa_tee_hashes_ctx.h"
#include "tee_hashes.h"

psa_status_t psa_hashes_sha256_setup(psa_hashes_sha256_ctx_t *ctx)
{
    return tee_hashes_sha256_setup((tee_hash_ctx_t *)ctx);
}

psa_status_t psa_hashes_sha256_update(psa_hashes_sha256_ctx_t *ctx,
                                      const uint8_t *input,
                                      size_t input_length)
{
    return tee_hashes_sha256_update((tee_hash_ctx_t *)ctx, input, input_length);
}

psa_status_t psa_hashes_sha256_finish(psa_hashes_sha256_ctx_t *ctx,
                                      uint8_t *hash,
                                      size_t hash_size,
                                      size_t *hash_length)
{
    return tee_hashes_sha256_finish((tee_hash_ctx_t *)ctx, hash, hash_size, hash_length);
}
