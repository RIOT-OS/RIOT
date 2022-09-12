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

#ifndef PSA_CRYPTO_INCLUDES_H
#define PSA_CRYPTO_INCLUDES_H

#ifdef __cplusplus
extern "C" {
#endif

#include "kernel_defines.h"

#if IS_ACTIVE(CONFIG_MODULE_PSA_RIOT_CIPHER)
#include "crypto/psa/riot_ciphers.h"
#endif

#if IS_ACTIVE(CONFIG_MODULE_PSA_RIOT_HASHES)
#include "hashes/psa/riot_hashes.h"
#endif

#if IS_ACTIVE(CONFIG_PERIPH_CIPHER_AES)
#include "psa_periph_aes_ctx.h"
#endif

#if IS_ACTIVE(CONFIG_PERIPH_HASHES)
#include "psa_periph_hashes_ctx.h"
#endif

#if IS_ACTIVE(CONFIG_PSA_SE_ATECCX08A)
#include "atca_params.h"
#include "atca.h"
#endif

#ifdef __cplusplus
}
#endif

#endif /* PSA_CRYPTO_INCLUDES_H */
/** @} */
