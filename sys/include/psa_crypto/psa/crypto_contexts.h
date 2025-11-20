/*
 * Copyright (C) 2021 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     sys_psa_crypto
 * @{
 *
 * @file        crypto_contexts.h
 * @brief       Context definitions for PSA Crypto
 *
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "kernel_defines.h"
#include "psa/crypto_includes.h"

#if IS_USED(MODULE_PSA_SECURE_ELEMENT)
/**
 * @brief   Structure containing the secure element specific cipher contexts needed by the
 *          application.
 */
typedef struct {
    psa_encrypt_or_decrypt_t direction; /**< Direction of this cipher operation */
    /** Structure containing a driver specific cipher context */
    union driver_context {
        unsigned dummy; /**< Make the union non-empty even with no supported algorithms. */
    #if IS_USED(MODULE_PSA_SECURE_ELEMENT_ATECCX08A) || defined(DOXYGEN)
        atca_aes_cbc_ctx_t atca_aes_cbc;    /**< ATCA AES CBC context*/
    #endif
    } drv_ctx;  /**< SE specific cipher operation context */
} psa_se_cipher_context_t;
#endif

#ifdef __cplusplus
}
#endif

/** @} */
