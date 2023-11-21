/*
 * Copyright (C) 2023 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

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

#ifndef PSA_CRYPTO_CBOR_ENCODER_H
#define PSA_CRYPTO_CBOR_ENCODER_H

#ifdef __cplusplus
extern "C" {
#endif

#include "psa_crypto_slot_management.h"

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

#endif /* PSA_CRYPTO_CBOR_ENCODER_H */
/** @} */
