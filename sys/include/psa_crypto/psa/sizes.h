/*
 * SPDX-FileCopyrightText: 2025 TU Dresden
 * SPDX-FileCopyrightText: 2021 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     sys_psa_crypto
 * @{
 *
 * @file        sizes.h
 * @brief       Size definitions for the PSA Crypto API
 *
 * @author      Armin Wolf <wolf.armin@mailbox.tu-dresden.de>
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Functions to convert bits to bytes
 *
 * @param   bits
 *
 * @return  Number of bytes contained in bits
 */
#define PSA_BITS_TO_BYTES(bits) (size_t)(((bits) + 7) / 8)

/**
 * @brief   Functions to convert bytes to bits
 *
 * @param   bytes
 *
 * @return  Number of bits contained in bytes
 */
#define PSA_BYTES_TO_BITS(bytes) ((bytes) * 8)

#ifdef __cplusplus
}
#endif

/** @} */
