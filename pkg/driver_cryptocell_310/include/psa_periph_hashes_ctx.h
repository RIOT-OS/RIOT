/*
 * Copyright (C) 2021 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     pkg_driver_cryptocell_310
 * @{
 *
 * @file
 * @brief       CryptoCell 310 driver specific hash contexts
 *
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include "kernel_defines.h"
#include "crys_hash.h"

#if IS_USED(MODULE_PERIPH_HASH_SHA_1) || DOXYGEN
/**
 * @brief   Map driver specific SHA1 context to PSA context
 */
typedef CRYS_HASHUserContext_t psa_hashes_sha1_ctx_t;
#endif

#if IS_USED(MODULE_PERIPH_HASH_SHA_224) || DOXYGEN
/**
 * @brief   Map driver specific SHA224 context to PSA context
 */
typedef CRYS_HASHUserContext_t psa_hashes_sha224_ctx_t;
#endif

#if IS_USED(MODULE_PERIPH_HASH_SHA_256) || DOXYGEN
/**
 * @brief   Map driver specific SHA256 context to PSA context
 */
typedef CRYS_HASHUserContext_t psa_hashes_sha256_ctx_t;
#endif

#if IS_USED(MODULE_PERIPH_HASH_SHA_512) || DOXYGEN
/**
 * @brief   Map driver specific SHA512 context to PSA context
 */
typedef CRYS_HASHUserContext_t psa_hashes_sha512_ctx_t;
#endif

#ifdef __cplusplus
}
#endif

/** @} */
