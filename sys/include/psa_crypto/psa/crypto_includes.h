/*
 * Copyright (C) 2021 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_psa_crypto
 * @{
 *
 * @brief       Files to include in the build of PSA Crypto
 *
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 */

#ifndef PSA_CRYPTO_PSA_CRYPTO_INCLUDES_H
#define PSA_CRYPTO_PSA_CRYPTO_INCLUDES_H

#ifdef __cplusplus
extern "C" {
#endif

#include "kernel_defines.h"

#if IS_USED(MODULE_CRYPTO)
#include "crypto/psa/riot_ciphers.h"
#endif

#if IS_USED(MODULE_PSA_RIOT_HASHES_HMAC_SHA256) || IS_USED(MODULE_PSA_RIOT_HASHES_MD5) || \
    IS_USED(MODULE_PSA_RIOT_HASHES_SHA_1) || IS_USED(MODULE_PSA_RIOT_HASHES_SHA_224) || \
    IS_USED(MODULE_PSA_RIOT_HASHES_SHA_256) || IS_USED(MODULE_PSA_RIOT_HASHES_SHA_512) || \
    IS_USED(MODULE_PSA_RIOT_HASHES_SHA_3_256) || IS_USED(MODULE_PSA_RIOT_HASHES_SHA_3_384) || \
    IS_USED(MODULE_PSA_RIOT_HASHES_SHA_3_512)
#include "hashes/psa/riot_hashes.h"
#endif

#if IS_USED(MODULE_PERIPH_CIPHER_AES_128_CBC)
#include "psa_periph_aes_ctx.h"
#endif

#if IS_USED(MODULE_PERIPH_HASH_SHA_1) || IS_USED(MODULE_PERIPH_HASH_SHA_224) || \
    IS_USED(MODULE_PERIPH_HASH_SHA_256) || IS_USED(MODULE_PERIPH_HASH_SHA_512)
#include "psa_periph_hashes_ctx.h"
#endif

#if IS_USED(MODULE_PSA_SECURE_ELEMENT_ATECCX08A)
#include "atca_params.h"
#endif

#ifdef __cplusplus
}
#endif

#endif /* PSA_CRYPTO_PSA_CRYPTO_INCLUDES_H */
/** @} */
