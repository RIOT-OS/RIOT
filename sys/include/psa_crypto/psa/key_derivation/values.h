/*
 * Copyright (C) 2025 TU Dresden
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
 * @file
 * @brief       Key derivation value definitions for the PSA Crypto API
 *
 * @author      Armin Wolf <wolf.armin@mailbox.tu-dresden.de>
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   A context for key derivation.
 *
 * @details This is typically a direct input. It can also be a key of type
 *          @ref PSA_KEY_TYPE_RAW_DATA.
 */
#define PSA_KEY_DERIVATION_INPUT_CONTEXT /* implementation-defined value */

/**
 * @brief   A cost parameter for password hashing or key stretching.
 *
 * @details This must be a direct input, passed to @ref psa_key_derivation_input_integer().
 */
#define PSA_KEY_DERIVATION_INPUT_COST /* implementation-defined value */

/**
 * @brief   An information string for key derivation.
 *
 * @details This is typically a direct input. It can also be a key of type
 *          @ref PSA_KEY_TYPE_RAW_DATA.
 */
#define PSA_KEY_DERIVATION_INPUT_INFO /* implementation-defined value */

/**
 * @brief   A label for key derivation.
 *
 * @details This is typically a direct input. It can also be a key of type
 *          @ref PSA_KEY_TYPE_RAW_DATA.
 */
#define PSA_KEY_DERIVATION_INPUT_LABEL /* implementation-defined value */

/**
 * @brief   A low-entropy secret input for password hashing or key stretching.
 *
 * @details This is usually a key of type @ref PSA_KEY_TYPE_PASSWORD passed to
 *          @ref psa_key_derivation_input_key() or a direct input passed to
 *          @ref psa_key_derivation_input_bytes() that is a password or passphrase. It can also be
 *          high-entropy secret, for example, a key of type @ref PSA_KEY_TYPE_DERIVE, or the shared
 *          secret resulting from a key agreement.
 *
 *          If the secret is a direct input, the derivation operation cannot be used to derive
 *          keys: the operation will not allow a call to @ref psa_key_derivation_output_key().
 */
#define PSA_KEY_DERIVATION_INPUT_PASSWORD /* implementation-defined value */

/**
 * @brief   A salt for key derivation.
 *
 * @details This is typically a direct input. It can also be a key of type
 *          @ref PSA_KEY_TYPE_RAW_DATA or @ref PSA_KEY_TYPE_PEPPER.
 */
#define PSA_KEY_DERIVATION_INPUT_SALT /* implementation-defined value */

/**
 * @brief   A high-entropy secret input for key derivation.
 *
 * @details This is typically a key of type @ref PSA_KEY_TYPE_DERIVE passed to
 *          @ref psa_key_derivation_input_key(), or the shared secret resulting from a key
 *          agreement obtained via @ref psa_key_derivation_key_agreement().
 *
 *          The secret can also be a direct input passed to @ref psa_key_derivation_input_bytes().
 *          In this case, the derivation operation cannot be used to derive keys: the operation
 *          will not allow a call to @ref psa_key_derivation_output_key().
 */
#define PSA_KEY_DERIVATION_INPUT_SECRET /* implementation-defined value */

/**
 * @brief   A seed for key derivation.
 *
 * @details This is typically a direct input. It can also be a key of type
 *          @ref PSA_KEY_TYPE_RAW_DATA.
 */
#define PSA_KEY_DERIVATION_INPUT_SEED /* implementation-defined value */

#ifdef __cplusplus
}
#endif

/** @} */
