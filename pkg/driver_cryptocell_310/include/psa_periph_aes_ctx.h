/*
 * SPDX-FileCopyrightText: 2021 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     pkg_driver_cryptocell_310
 * @{
 *
 * @file
 * @brief       CryptoCell 310 driver specific AES contexts
 *
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "ssi_aes.h"
#include "kernel_defines.h"

#if IS_USED(MODULE_PERIPH_CIPHER_AES_128_CBC) || DOXYGEN
/**
 * @brief   Map driver specific AES context to PSA context
 */
typedef SaSiAesUserContext_t psa_cipher_aes_128_ctx_t;
#endif

#ifdef __cplusplus
}
#endif

/** @} */
