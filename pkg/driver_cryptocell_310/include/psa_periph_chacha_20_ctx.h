/*
 * Copyright (C) 2024 TU Dresden
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
 * @brief       CryptoCell 310 driver specific ChaCha20 contexts
 *
 * @author      Lennard Melling <lennard.melling@msx.tu-dresden.de>
 *
 */
#ifndef PSA_PERIPH_CHACHA20_CTX_H
#define PSA_PERIPH_CHACHA20_CTX_H

#ifdef __cplusplus
extern "C" {
#endif

#include "crys_chacha.h"
#include "kernel_defines.h"

#if IS_USED(MODULE_PERIPH_CIPHER_CHACHA_20) || DOXYGEN
/**
 * @brief   Map driver specific ChaCha20 context to PSA context
 */
typedef CRYS_CHACHAUserContext_t psa_cipher_chacha_20_ctx_t;

/**
 * @brief Map driver specific ChaCha20 nonce to PSA nonce
 */
typedef CRYS_CHACHA_Nonce_t psa_cipher_chacha_20_nonce_t;
#endif

#ifdef __cplusplus
}
#endif

#endif /* PSA_PERIPH_CHACHA20_CTX_H */
/** @} */
