/*
 * SPDX-FileCopyrightText: 2024 TU Dresden
 * SPDX-FileCopyrightText: 2021 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     sys_psa_crypto
 * @{
 *
 * @file        algorithm.h
 * @brief       Algorithm definitions for the PSA Crypto API
 *
 * @author      Armin Wolf <wolf.armin@mailbox.tu-dresden.de>
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**
 * @brief   Encoding of a cryptographic algorithm.
 *
 * @details For algorithms that can be applied to multiple key types, this identifier does not
 *          encode the key type. For example, for symmetric ciphers based on a block cipher,
 *          @ref psa_algorithm_t encodes the block cipher mode and the padding mode while the
 *          block cipher itself is encoded via @ref psa_key_type_t.
 */
typedef uint32_t psa_algorithm_t;

/**
 * @brief   An invalid algorithm identifier value.
 *
 * @details Zero is not the encoding of any algorithm.
 */
#define PSA_ALG_NONE ((psa_algorithm_t)0)

/**
 * @brief   PSA algorithm category mask.
 */
#define PSA_ALG_CATEGORY_MASK ((psa_algorithm_t)0x7f000000)

#ifdef __cplusplus
}
#endif

/** @} */
