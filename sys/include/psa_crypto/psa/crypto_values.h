/*
 * Copyright (C) 2021 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     sys_psa_crypto
 * @{
 *
 * @file        crypto_values.h
 * @brief       Value definitions for PSA Crypto.
 *
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 * @note        Some of the macros in this file have already been copied here from
 *              the PSA API specification, but are not implemented, yet.
 *              They are marked by comments that either say "specification-defined"
 *              or "implementation-defined".
 *              These macros will be implemented successively in the future.
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "crypto_types.h"
#include "psa/hash/algorithm.h"
#include "psa/mac/algorithm.h"
#include "psa/cipher/algorithm.h"
#include "psa/aead/algorithm.h"
#include "psa/key_agreement/algorithm.h"
#include "psa/key_derivation/algorithm.h"
#include "psa/key_derivation/values.h"
#include "psa/asymmetric_encryption/algorithm.h"
#include "psa/asymmetric_signature/algorithm.h"

/**
 * @brief   A sufficient signature buffer size for @ref psa_sign_message() and
 *          @ref psa_sign_hash(), for any of the supported key types and asymmetric signature
 *          algorithms.
 *
 * @details If the size of the signature buffer is at least this large, it is guaranteed that
 *          @ref psa_sign_message() and @ref psa_sign_hash() will not fail due to an insufficient
 *          buffer size.
 *
 *          See also @ref PSA_SIGN_OUTPUT_SIZE().
 */
#define PSA_SIGNATURE_MAX_SIZE /* implementation-defined value */

#ifdef __cplusplus
}
#endif

/** @} */
