/*
 * SPDX-FileCopyrightText: 2021 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     sys_psa_crypto
 * @{
 *
 * @file        crypto_types.h
 * @brief       Type definitions for PSA Crypto
 *
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#include "psa/algorithm.h"
#include "psa/error.h"
#include "psa/key/attributes.h"

#include "aead/types.h"
#include "cipher/types.h"
#include "hash/types.h"
#include "key_derivation/types.h"
#include "mac/types.h"

#ifdef __cplusplus
}
#endif

/** @} */
