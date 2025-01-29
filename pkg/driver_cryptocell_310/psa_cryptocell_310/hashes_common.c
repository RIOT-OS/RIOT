/*
 * Copyright (C) 2021 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_driver_cryptocell_310
 * @{
 *
 * @brief       Common hash functions used by all PSA Crypto wrappers
 *              for the CryptoCell 310 hash APIs.
 *
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 * @}
 */

#include "psa/crypto.h"
#include "psa_error.h"
#include "cryptocell_310_util.h"
#include "crys_hash.h"

#define ENABLE_DEBUG    0
#include "debug.h"

#define CC310_MAX_HASH_INPUT_BLOCK       (0xFFF0)

psa_status_t cryptocell_310_common_hash_setup(CRYS_HASHUserContext_t *ctx,
                               CRYS_HASH_OperationMode_t mode)
{
    CRYSError_t ret = CRYS_HASH_Init(ctx, mode);

    if (ret != CRYS_OK) {
        DEBUG("CRYS_HASH_Init failed with %s\n", cryptocell310_status_to_humanly_readable(ret));
        return CRYS_to_psa_error(ret);
    }
    return PSA_SUCCESS;
}

psa_status_t cryptocell_310_common_hash_update(CRYS_HASHUserContext_t *ctx,
                                const uint8_t *input,
                                size_t input_length)
{
    CRYSError_t ret = 0;
    size_t offset = 0;
    size_t size;

    if (!cryptocell_310_data_within_ram(input)) {
        DEBUG("%s : cryptocell_310 data required to be in RAM.\n", RIOT_FILE_RELATIVE);
        return PSA_ERROR_DATA_INVALID;
    }

    do {
        if (input_length > CC310_MAX_HASH_INPUT_BLOCK) {
            size = CC310_MAX_HASH_INPUT_BLOCK;
            input_length -= CC310_MAX_HASH_INPUT_BLOCK;
        }
        else {
            size = input_length;
            input_length = 0;
        }

        cryptocell_310_enable();
        ret = CRYS_HASH_Update(ctx, (uint8_t *)(input + offset), size);
        cryptocell_310_disable();

        offset += size;
    } while ((input_length > 0) && (ret == CRYS_OK));

    if (ret != CRYS_OK) {
        DEBUG("CRYS_HASH_Update failed with %s\n", cryptocell310_status_to_humanly_readable(ret));
        return CRYS_to_psa_error(ret);
    }
    return PSA_SUCCESS;
}

psa_status_t cryptocell_310_common_hash_finish(CRYS_HASHUserContext_t *ctx, uint8_t *hash, size_t hash_size,
                                size_t *hash_length)
{
    cryptocell_310_enable();
    CRYSError_t ret = CRYS_HASH_Finish(ctx, (uint32_t *)hash);
    cryptocell_310_disable();
    if (ret != CRYS_OK) {
        DEBUG("CRYS_HASH_Finish failed with %s\n", cryptocell310_status_to_humanly_readable(ret));
        return CRYS_to_psa_error(ret);
    }
    *hash_length = hash_size;
    return PSA_SUCCESS;
}
