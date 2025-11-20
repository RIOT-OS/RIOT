/*
 * Copyright (C) 2023 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @ingroup     sys_psa_crypto
 * @defgroup    sys_psa_crypto_cbor_encoder     Module for encoding PSA keys in CBOR
 * @{
 *
 * @file        psa_crypto_cbor_encoder.h
 * @brief
 *
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 */

#ifdef __cplusplus
extern "C" {
#endif

#include "psa_crypto_slot_management.h"

/**
 * @brief   Required size of CBOR output buffer from start to end of attributes.
 *          Adds attributes sizes to CBOR encodings for individual values.
 */
#define CBOR_BUF_SIZE_START         (   1 + /* Array encoding */ \
                                        1 + /* Array encoding */ \
                                        1 + sizeof(psa_key_id_t) + \
                                        1 + sizeof(psa_key_type_t) + \
                                        1 + sizeof(psa_key_bits_t) + \
                                        1 + sizeof(psa_key_lifetime_t) + \
                                        1 + /* Array encoding */ \
                                        1 + sizeof(psa_key_usage_t) + \
                                        1 + sizeof(psa_algorithm_t) \
                                    )

#if PSA_SINGLE_KEY_COUNT
/**
 * @brief   Required CBOR buffer size to encode a basic PSA key slot containing
 *          a single key.
 */
#define CBOR_BUF_SIZE_SINGLE_KEY    (   CBOR_BUF_SIZE_START + \
                                        3 + /* Bytestring encoding and size */ \
                                        PSA_MAX_KEY_DATA_SIZE \
                                    )
#endif /* PSA_SINGLE_KEY_COUNT */

#if PSA_ASYMMETRIC_KEYPAIR_COUNT
/**
 * @brief   Required CBOR buffer size to encode a basic PSA key slot containing
 *          an asymmetric key pair.
 */
#define CBOR_BUF_SIZE_KEY_PAIR      (   CBOR_BUF_SIZE_START + \
                                        1 + \
                                        3 + PSA_BITS_TO_BYTES(PSA_MAX_PRIV_KEY_SIZE) + \
                                        3 + PSA_EXPORT_PUBLIC_KEY_MAX_SIZE \
                                    )
#endif /* PSA_ASYMMETRIC_KEYPAIR_COUNT */

#if PSA_PROTECTED_KEY_COUNT && IS_USED(MODULE_PSA_ASYMMETRIC)
/**
 * @brief   Required CBOR buffer size to encode a basic PSA key slot containing
 *          a key in protected memory.
 */
#define CBOR_BUF_SIZE_PROT_KEY      (   CBOR_BUF_SIZE_START + \
                                        1 + \
                                        1 + sizeof(psa_key_slot_number_t) + \
                                        3 + PSA_EXPORT_PUBLIC_KEY_MAX_SIZE \
                                    )
#elif PSA_PROTECTED_KEY_COUNT
/**
 * @brief   Required CBOR buffer size to encode a basic PSA key slot containing
 *          a key in protected memory.
 */
#define CBOR_BUF_SIZE_PROT_KEY      (   CBOR_BUF_SIZE_START + \
                                        1 + \
                                        1 + sizeof(psa_key_slot_number_t) \
                                    )
#endif /* PSA_PROTECTED_KEY_COUNT */

/**
 * @brief   Encodes a basic key slot in CBOR
 *
 *          Single Key Format:
 *          -   [
 *                  [ID, Type, Bits, Lifetime, [Usage, Algorithm]],
 *                  h'key
 *              ]
 *
 *          Asymmetric Key Pair Format:
 *          -   [
 *                  [ID, Type, Bits, Lifetime, [Usage, Algorithm]],
 *                  [h'private_key, h'public_key]
 *              ]
 *
 *          Protected Key Format:
 *          -   [
 *                  [ID, Type, Bits, Lifetime, [Usage, Algorithm]],
 *                  [Slot No, *optional: h'public_key*]
 *              ]
 *
 * @param[in] slot           Pointer to slot containing the key to encode
 * @param[in] output         Buffer to write the encoded key to
 * @param[in] output_len     Length of output buffer
 * @param[out] output_size   Pointer to write actual length of encoding
 *
 * @return psa_status_t
 */
psa_status_t psa_encode_key_slot(psa_key_slot_t *slot, uint8_t *output,
                                     size_t output_len, size_t *output_size);

/**
 * @brief   Decode CBOR encoded key data and write to PSA key slot. Only decodes the key and should
 *          be called in combination with psa_decode_key_attributes.
 *
 * @param slot          Pointer to key slot to write decoded key to
 * @param cbor_buf      Buffer containing CBOR encoded data
 * @param cbor_buf_size Size of @p cbor_buf
 * @return psa_status_t
 */
psa_status_t psa_decode_key_slot_data(psa_key_slot_t *slot, uint8_t *cbor_buf,
                                      size_t cbor_buf_size);

/**
 * @brief   Decode CBOR PSA key attributes. Only decodes key attributes and not the actual key.
 *          Key can be decoded with psa_decode_key_slot_data.
 *
 * @param   attr            Key attribute struct to store decoded attributes
 * @param   cbor_buf        Buffer containing CBOR encoded data
 * @param   cbor_buf_size    Size of @p cbor_buf
 * @return  psa_status_t
 */
psa_status_t psa_decode_key_attributes(psa_key_attributes_t *attr, uint8_t *cbor_buf,
                                       size_t cbor_buf_size);

#ifdef __cplusplus
}
#endif

/** @} */
