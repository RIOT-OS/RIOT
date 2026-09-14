/*
 * SPDX-FileCopyrightText: 2022 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     sys_psa_crypto
 * @{
 *
 * @brief       Contexts for the RIOT cipher module
 *
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "crypto/ciphers.h"
#include "kernel_defines.h"

#if IS_USED(MODULE_PSA_RIOT_CIPHER_AES_128_CBC)
typedef cipher_t psa_cipher_aes_128_ctx_t;
#endif
#if IS_USED(MODULE_PSA_RIOT_CIPHER_AES_192_CBC)
typedef cipher_t psa_cipher_aes_192_ctx_t;
#endif
#if IS_USED(MODULE_PSA_RIOT_CIPHER_AES_256_CBC)
typedef cipher_t psa_cipher_aes_256_ctx_t;
#endif

#ifdef __cplusplus
}
#endif

/** @} */
