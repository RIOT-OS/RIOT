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
 * @file        key/bits.h
 * @brief       Key size definitions for the PSA Crypto API
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
 * @brief   Public interfaces use @c size_t, but internally we use a smaller type.
 */
typedef uint16_t psa_key_bits_t;

#ifdef __cplusplus
}
#endif

/** @} */
