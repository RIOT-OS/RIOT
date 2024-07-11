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
 * @brief       CryptoCell 310 driver specific hash contexts
 *
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 */

#ifndef PSA_TEE_HASHES_CTX_H
#define PSA_TEE_HASHES_CTX_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdlib.h>
#include "kernel_defines.h"
#include "tee_hashes.h"

#if IS_USED(MODULE_PSA_TEE_HASHES_SHA256) || DOXYGEN
/**
 * @brief   Map driver specific SHA256 context to PSA context
 */
typedef tee_hash_ctx_t psa_hashes_sha256_ctx_t;
#endif

#ifdef __cplusplus
}
#endif

#endif /* PSA_TEE_HASHES_CTX_H */
/** @} */
