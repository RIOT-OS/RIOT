/*
 * Copyright (C) 2023 TU Dresden
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
 * @author      Mikolai Gütschow <mikolai.guetschow@tu-dresden.de>
 *
 * @}
 */

#include "psa/crypto.h"
#include "hashes/psa/riot_hashes.h"

psa_status_t psa_hashes_sha512_224_setup(psa_hashes_sha512_224_ctx_t *ctx)
{
    sha512_224_init((sha512_224_context_t *)ctx);
    return PSA_SUCCESS;
}

psa_status_t psa_hashes_sha512_224_update(psa_hashes_sha512_224_ctx_t *ctx,
                                      const uint8_t *input,
                                      size_t input_length)
{
    sha512_224_update((sha512_224_context_t *)ctx, input, input_length);
    return PSA_SUCCESS;
}

psa_status_t psa_hashes_sha512_224_finish(psa_hashes_sha512_224_ctx_t *ctx,
                                      uint8_t *hash,
                                      size_t hash_size,
                                      size_t *hash_length)
{
    sha512_224_final((sha512_224_context_t *)ctx, hash);

    (void)hash_size;
    (void)hash_length;
    return PSA_SUCCESS;
}
