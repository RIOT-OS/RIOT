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
 * @brief       Contexts for the RIOT cipher module
 *
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 */

#ifndef CRYPTO_PSA_RIOT_CIPHERS_H
#define CRYPTO_PSA_RIOT_CIPHERS_H

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
#endif /* CRYPTO_PSA_RIOT_CIPHERS_H */
