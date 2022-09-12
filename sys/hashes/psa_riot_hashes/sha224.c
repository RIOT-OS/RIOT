/*
 * Copyright (C) 2022 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_psa_crypto
 * @{
 *
 * @brief       Glue code translating between PSA Crypto and the RIOT Hash module
 *
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 * @}
 */

#include "psa/crypto.h"
#include "hashes/psa/riot_hashes.h"

psa_status_t psa_hashes_sha224_setup(psa_hashes_sha224_ctx_t *ctx)
{
    sha224_init((sha224_context_t *)ctx);
    return PSA_SUCCESS;
}

psa_status_t psa_hashes_sha224_update(psa_hashes_sha224_ctx_t *ctx,
                                      const uint8_t *input,
                                      size_t input_length)
{
    sha224_update((sha224_context_t *)ctx, input, input_length);
    return PSA_SUCCESS;
}

psa_status_t psa_hashes_sha224_finish(psa_hashes_sha224_ctx_t *ctx,
                                      uint8_t *hash,
                                      size_t hash_size,
                                      size_t *hash_length)
{
    sha224_final((sha224_context_t *)ctx, hash);

    (void)hash_size;
    (void)hash_length;
    return PSA_SUCCESS;
}
