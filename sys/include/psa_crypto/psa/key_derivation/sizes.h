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
 * @file
 * @brief       Key derivation size definitions for the PSA Crypto API
 *
 * @author      Armin Wolf <wolf.armin@mailbox.tu-dresden.de>
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Use the maximum possible capacity for a key derivation operation.
 *
 * @details Use this value as the capacity argument when setting up a key derivation to specify
 *          that the operation will use the maximum possible capacity. The value of the maximum
 *          possible capacity depends on the key derivation algorithm.
 */
#define PSA_KEY_DERIVATION_UNLIMITED_CAPACITY \
/* implementation-defined value */

/**
 * @brief   This macro returns the maximum supported length of the PSK for the TLS-1.2 PSK-to-MS
 *          key derivation.
 *
 * @details This implementation-defined value specifies the maximum length for the PSK input used
 *          with a @ref PSA_ALG_TLS12_PSK_TO_MS() key agreement algorithm.
 *
 *          Quoting Pre-Shared Key Ciphersuites for Transport Layer Security (TLS)
 *          [RFC4279](https://tools.ietf.org/html/rfc4279.html) §5.3:
 *          TLS implementations supporting these cipher suites MUST support arbitrary PSK
 *          identities up to 128 octets in length, and arbitrary PSKs up to 64 octets in length.
 *          Supporting longer identities and keys is RECOMMENDED.
 *
 *          Therefore, it is recommended that implementations define
 *          @ref PSA_TLS12_PSK_TO_MS_PSK_MAX_SIZE with a value greater than or equal to 64.
 */
#define PSA_TLS12_PSK_TO_MS_PSK_MAX_SIZE /* implementation-defined value */

#ifdef __cplusplus
}
#endif

/** @} */
