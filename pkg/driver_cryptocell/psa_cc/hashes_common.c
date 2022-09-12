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
 * @brief       Glue code translating between PSA Crypto and the CryptoCell 310 driver APIs
 *
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 * @}
 */

#include "psa/crypto.h"
#include "psa_error.h"
#include "cryptocell_util.h"
#include "crys_hash.h"

#define ENABLE_DEBUG    0
#include "debug.h"

#define CC310_MAX_HASH_INPUT_BLOCK       (0xFFF0)

psa_status_t common_hash_setup(CRYS_HASHUserContext_t *ctx,
                               CRYS_HASH_OperationMode_t mode)
{
    int ret = CRYS_HASH_Init(ctx, mode);

    if (ret != CRYS_OK) {
        return CRYS_to_psa_error(ret);
    }
    return PSA_SUCCESS;
}

psa_status_t common_hash_update(CRYS_HASHUserContext_t *ctx,
                                const uint8_t *input,
                                size_t input_length)
{
    int ret = 0;
    size_t offset = 0;
    size_t size;

    do {
        if (input_length > CC310_MAX_HASH_INPUT_BLOCK) {
            size = CC310_MAX_HASH_INPUT_BLOCK;
            input_length -= CC310_MAX_HASH_INPUT_BLOCK;
        }
        else {
            size = input_length;
            input_length = 0;
        }

        cryptocell_enable();
        ret = CRYS_HASH_Update(ctx, (uint8_t *)(input + offset), size);
        cryptocell_disable();

        offset += size;
    } while ((input_length > 0) && (ret == CRYS_OK));

    if (ret != CRYS_OK) {
        return CRYS_to_psa_error(ret);
    }
    return PSA_SUCCESS;
}

psa_status_t common_hash_finish(CRYS_HASHUserContext_t *ctx, uint8_t *hash, size_t hash_size,
                                size_t *hash_length)
{
    cryptocell_enable();
    int ret = CRYS_HASH_Finish(ctx, (uint32_t *)hash);
    cryptocell_disable();
    if (ret != CRYS_OK) {
        return CRYS_to_psa_error(ret);
    }
    *hash_length = hash_size;
    return PSA_SUCCESS;
}
