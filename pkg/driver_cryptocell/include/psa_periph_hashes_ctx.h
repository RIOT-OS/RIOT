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
 * @brief       CryptoCell 310 driver specific hash contexts
 *
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 */

#ifndef PSA_PERIPH_HASHES_CTX_H
#define PSA_PERIPH_HASHES_CTX_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include "crys_hash.h"

#if IS_ACTIVE(CONFIG_PERIPH_HASH_SHA1)
/**
 * @brief   Map driver specific SHA1 context to PSA context
 */
typedef CRYS_HASHUserContext_t psa_hashes_sha1_ctx_t;
#endif

#if IS_ACTIVE(CONFIG_PERIPH_HASH_SHA224)
/**
 * @brief   Map driver specific SHA224 context to PSA context
 */
typedef CRYS_HASHUserContext_t psa_hashes_sha224_ctx_t;
#endif

#if IS_ACTIVE(CONFIG_PERIPH_HASH_SHA256)
/**
 * @brief   Map driver specific SHA256 context to PSA context
 */
typedef CRYS_HASHUserContext_t psa_hashes_sha256_ctx_t;
#endif

#if IS_ACTIVE(CONFIG_PERIPH_HASH_SHA512)
/**
 * @brief   Map driver specific SHA512 context to PSA context
 */
typedef CRYS_HASHUserContext_t psa_hashes_sha512_ctx_t;
#endif

#ifdef __cplusplus
}
#endif

#endif /* PSA_PERIPH_HASHES_CTX_H */
/** @} */
