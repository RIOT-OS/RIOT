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
 * @file
 * @brief       Glue code translating between PSA Crypto and the CryptoCell 310 driver APIs
 *
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 * @}
 */

#include "psa_error.h"

psa_status_t CRYS_to_psa_error(CRYSError_t error)
{
    switch (error) {
    case CRYS_HASH_ILLEGAL_OPERATION_MODE_ERROR:
    case CRYS_HASH_IS_NOT_SUPPORTED:
        return PSA_ERROR_NOT_SUPPORTED;
    case CRYS_HASH_USER_CONTEXT_CORRUPTED_ERROR:
        return PSA_ERROR_CORRUPTION_DETECTED;
    case CRYS_HASH_DATA_IN_POINTER_INVALID_ERROR:
    case CRYS_HASH_DATA_SIZE_ILLEGAL:
        return PSA_ERROR_DATA_INVALID;
    case CRYS_HASH_INVALID_RESULT_BUFFER_POINTER_ERROR:
    case CRYS_HASH_ILLEGAL_PARAMS_ERROR:
    case CRYS_HASH_INVALID_USER_CONTEXT_POINTER_ERROR:
    case CRYS_HASH_LAST_BLOCK_ALREADY_PROCESSED_ERROR:
    case CRYS_HASH_CTX_SIZES_ERROR:
        return PSA_ERROR_INVALID_ARGUMENT;
    default:
        return PSA_ERROR_GENERIC_ERROR;
    }
}

psa_status_t SaSi_to_psa_error(SaSiStatus error)
{
    switch (error) {
    case SASI_AES_INVALID_USER_CONTEXT_POINTER_ERROR:
    case SASI_AES_INVALID_IV_OR_TWEAK_PTR_ERROR:
    case SASI_AES_ILLEGAL_OPERATION_MODE_ERROR:
    case SASI_AES_ILLEGAL_KEY_SIZE_ERROR:
    case SASI_AES_INVALID_KEY_POINTER_ERROR:
    case SASI_AES_INVALID_ENCRYPT_MODE_ERROR:
    case SASI_AES_DATA_IN_POINTER_INVALID_ERROR:
    case SASI_AES_DATA_OUT_POINTER_INVALID_ERROR:
    case SASI_AES_DATA_IN_SIZE_ILLEGAL:
    case SASI_AES_DATA_OUT_DATA_IN_OVERLAP_ERROR:
    case SASI_AES_DATA_OUT_SIZE_POINTER_INVALID_ERROR:
    case SASI_AES_CTX_SIZES_ERROR:
    case SASI_AES_ILLEGAL_PARAMS_ERROR:
    case SASI_AES_CTR_ILLEGAL_BLOCK_OFFSET_ERROR:
    case SASI_AES_CTR_ILLEGAL_COUNTER_ERROR:
        return PSA_ERROR_INVALID_ARGUMENT;
    case SASI_AES_DATA_IN_BUFFER_SIZE_ERROR:
    case SASI_AES_DATA_OUT_BUFFER_SIZE_ERROR:
        return PSA_ERROR_BUFFER_TOO_SMALL;
    case SASI_AES_ILLEGAL_PADDING_TYPE_ERROR:
    case SASI_AES_INCORRECT_PADDING_ERROR:
        return PSA_ERROR_INVALID_PADDING;
    case SASI_AES_CORRUPTED_OUTPUT_ERROR:
    case SASI_AES_USER_CONTEXT_CORRUPTED_ERROR:
        return PSA_ERROR_CORRUPTION_DETECTED;
    case SASI_AES_DECRYPTION_NOT_ALLOWED_ON_THIS_MODE:
    case SASI_AES_ADDITIONAL_BLOCK_NOT_PERMITTED_ERROR:
        return PSA_ERROR_NOT_PERMITTED;
    case SASI_AES_KEY_TYPE_NOT_SUPPORTED_ERROR:
    case SASI_AES_IS_NOT_SUPPORTED:
        return PSA_ERROR_NOT_SUPPORTED;
    case SASI_OUT_OF_RESOURCE_ERROR:
        return PSA_ERROR_INSUFFICIENT_MEMORY;
    default:
        return PSA_ERROR_GENERIC_ERROR;
    }
}
