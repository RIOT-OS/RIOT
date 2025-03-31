/*
 * Copyright (C) 2021 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef PSA_CRYPTO_PSA_CRYPTO_VALUES_H
#define PSA_CRYPTO_PSA_CRYPTO_VALUES_H

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
#include "psa/asymmetric_encryption/algorithm.h"
#include "psa/asymmetric_signature/algorithm.h"

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
 * @brief   Sufficient output buffer size for @ref psa_raw_key_agreement(), for any of the
 *          supported key types and key agreement algorithms.
 *
 * @details If the size of the output buffer is at least this large, it is guaranteed that
 *          @ref psa_raw_key_agreement() will not fail due to an insufficient buffer size.
 *
 *          See also @ref PSA_RAW_KEY_AGREEMENT_OUTPUT_SIZE().
 */
#define PSA_RAW_KEY_AGREEMENT_OUTPUT_MAX_SIZE \
/* implementation-defined value */

/**
 * @brief   Sufficient output buffer size for @ref psa_raw_key_agreement().
 *
 * @details If the size of the output buffer is at least this large, it is guaranteed that
 *          @ref psa_raw_key_agreement() will not fail due to an insufficient buffer size.
 *          The actual size of the output might be smaller in any given call.
 *
 *          See also @ref PSA_RAW_KEY_AGREEMENT_OUTPUT_MAX_SIZE.
 *
 * @param   key_type A supported key type.
 * @param   key_bits The size of the key in bits.
 *
 * @return  A sufficient output buffer size for the specified key type and size.
 *          0 if key type is not supported.
 *          If the parameters are not valid, the return value is unspecified.
 */
#define PSA_RAW_KEY_AGREEMENT_OUTPUT_SIZE(key_type, key_bits) \
/* implementation-defined value */

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
#endif /* PSA_CRYPTO_PSA_CRYPTO_VALUES_H */
