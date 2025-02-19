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

static psa_status_t _hash_update_from_ram(CRYS_HASHUserContext_t *ctx,
                                const uint8_t *input,
                                size_t input_length)
{
    CRYSError_t ret = 0;
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

        cryptocell_310_enable();
        ret = CRYS_HASH_Update(ctx, (uint8_t *)(input + offset), size);
        cryptocell_310_disable();

        offset += size;
    } while ((input_length > 0) && (ret == CRYS_OK));

    return ret;
}

psa_status_t cryptocell_310_common_hash_update(CRYS_HASHUserContext_t *ctx,
                                const uint8_t *input,
                                size_t input_length)
{
    CRYSError_t ret = 0;
    if (!cryptocell_310_data_within_ram(input)) {
        /* copy input chuckwise into RAM until the end of input is reached */
        uint8_t input_buf[PSA_HASH_MAX_SIZE];
        size_t buf_size;
        size_t offset = 0;

        do {

            memset(&input_buf, 0, PSA_HASH_MAX_SIZE);

            buf_size = input_length - offset;
            buf_size = buf_size < PSA_HASH_MAX_SIZE ? buf_size : PSA_HASH_MAX_SIZE;

            memcpy(&input_buf, input + offset, buf_size);

            ret = _hash_update_from_ram(ctx, input_buf, buf_size);

            offset += PSA_HASH_MAX_SIZE;

        } while ((offset < input_length) && (ret == CRYS_OK));

    }
    else {
        ret = _hash_update_from_ram(ctx, input, input_length);
    }

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
