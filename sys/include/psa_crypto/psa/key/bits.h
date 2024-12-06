/*
 * Copyright (C) 2024 TU Dresden
 * Copyright (C) 2021 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_psa_crypto
 * @{
 *
 * @file        key/bits.h
 * @brief       Key size definitions for the PSA Crypto API
 *
 * @author      Armin Wolf <wolf.armin@mailbox.tu-dresden.de>
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 */

#ifndef PSA_CRYPTO_PSA_KEY_BITS_H
#define PSA_CRYPTO_PSA_KEY_BITS_H

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**
 * @brief   Public interfaces use @c size_t, but internally we use a smaller type.
 */
typedef uint16_t psa_key_bits_t;

#ifdef __cplusplus
}
#endif

#endif /* PSA_CRYPTO_PSA_KEY_BITS_H */
/** @} */
