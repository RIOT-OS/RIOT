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
 * @author      Lennard Melling <lennard.melling@tu-dresden.de>
 *
 * @}
 */

#include "psa/crypto.h"
#include "hashes/psa/riot_hashes.h"

psa_status_t psa_hashes_sha3_256_setup(psa_hashes_sha3_ctx_t *ctx)
{
    sha3_256_init((keccak_state_t *)ctx);
    return PSA_SUCCESS;
}

psa_status_t psa_hashes_sha3_update(psa_hashes_sha3_ctx_t *ctx,
                                      const uint8_t *input,
                                      size_t input_length)
{
    sha3_update((keccak_state_t *)ctx, input, input_length);
    return PSA_SUCCESS;
}

psa_status_t psa_hashes_sha3_256_finish(psa_hashes_sha3_ctx_t *ctx,
                                      uint8_t *hash,
                                      size_t hash_size,
                                      size_t *hash_length)
{
    sha3_256_final((keccak_state_t *)ctx, hash);

    (void)hash_size;
    (void)hash_length;
    return PSA_SUCCESS;
}

psa_status_t psa_hashes_sha3_384_setup(psa_hashes_sha3_ctx_t *ctx)
{
    sha3_384_init((keccak_state_t *)ctx);
    return PSA_SUCCESS;
}

psa_status_t psa_hashes_sha3_384_finish(psa_hashes_sha3_ctx_t *ctx,
                                      uint8_t *hash,
                                      size_t hash_size,
                                      size_t *hash_length)
{
    sha3_384_final((keccak_state_t *)ctx, hash);

    (void)hash_size;
    (void)hash_length;
    return PSA_SUCCESS;
}

psa_status_t psa_hashes_sha3_512_setup(psa_hashes_sha3_ctx_t *ctx)
{
    sha3_512_init((keccak_state_t *)ctx);
    return PSA_SUCCESS;
}

psa_status_t psa_hashes_sha3_512_finish(psa_hashes_sha3_ctx_t *ctx,
                                      uint8_t *hash,
                                      size_t hash_size,
                                      size_t *hash_length)
{
    sha3_512_final((keccak_state_t *)ctx, hash);

    (void)hash_size;
    (void)hash_length;
    return PSA_SUCCESS;
}
