/*
 * SPDX-FileCopyrightText: 2021 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     sys_psa_crypto
 * @{
 *
 * @file        crypto_sizes.h
 * @brief       Size definitions for PSA Crypto
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

#include "kernel_defines.h"
#include "crypto_values.h"

#include "aead/sizes.h"
#include "asymmetric_encryption/sizes.h"
#include "asymmetric_signature/sizes.h"
#include "cipher/sizes.h"
#include "hash/sizes.h"
#include "key/sizes.h"
#include "key/values.h"
#include "key_agreement/sizes.h"
#include "key_derivation/sizes.h"
#include "mac/sizes.h"
#include "sizes.h"

#ifdef __cplusplus
}
#endif

/** @} */
