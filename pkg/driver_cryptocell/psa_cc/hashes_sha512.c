/*
 * Copyright (C) 2021 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_psa_crypto pkg_driver_cryptocell
 * @{
 *
 * @brief       Glue code for ARM Cryptocell driver support in PSA Crypto
 *
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 * @}
 */

#include <stdio.h>
#include "kernel_defines.h"
#include "psa_periph_hashes_ctx.h"
#include "psa_periph_hashes_common.h"

psa_status_t psa_hashes_sha512_setup(psa_hashes_sha512_ctx_t *ctx)
{
    return common_hash_setup((CRYS_HASHUserContext_t *)ctx, CRYS_HASH_SHA512_mode);
}

psa_status_t psa_hashes_sha512_update(psa_hashes_sha512_ctx_t *ctx,
                                      const uint8_t *input,
                                      size_t input_length)
{
    return common_hash_update((CRYS_HASHUserContext_t *)ctx, input, input_length);
}

psa_status_t psa_hashes_sha512_finish(  psa_hashes_sha512_ctx_t *ctx,
                                        uint8_t *hash,
                                        size_t hash_size,
                                        size_t *hash_length)
{
    return common_hash_finish((CRYS_HASHUserContext_t *)ctx, hash, hash_size, hash_length);
}
