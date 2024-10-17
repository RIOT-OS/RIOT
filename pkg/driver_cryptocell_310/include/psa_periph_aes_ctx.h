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
 * @file
 * @brief       CryptoCell 310 driver specific AES contexts
 *
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 */
#ifndef PSA_PERIPH_AES_CTX_H
#define PSA_PERIPH_AES_CTX_H

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
#endif /* PSA_PERIPH_AES_CTX_H */
