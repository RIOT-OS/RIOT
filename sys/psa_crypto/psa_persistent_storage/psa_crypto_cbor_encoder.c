/*
 * Copyright (C) 2023 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     sys_psa_crypto sys_psa_crypto_slot_mgmt
 * @{
 *
 * @file
 * @brief       API to encode PSA Crypto keys to CBOR for persistent storage
 *
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 * @}
 */

#include "nanocbor/nanocbor.h"
#include "psa_crypto_slot_management.h"

#define ENABLE_DEBUG    0
#include "debug.h"

/**
 * @brief   Convert a nanocbor error to a string for debugging
 *
 * @param   res Nanocbor error value
 * @return  const char*
 */
static const char *nanocbor_error_to_string(int res)
{
    switch (res) {
        case NANOCBOR_ERR_OVERFLOW:
            return "NANOCBOR_ERR_OVERFLOW";
        case NANOCBOR_ERR_INVALID_TYPE:
            return "NANOCBOR_ERR_INVALID_TYPE";
        case NANOCBOR_ERR_END:
            return "NANOCBOR_ERR_END";
        case NANOCBOR_ERR_RECURSION:
            return "NANOCBOR_ERR_RECURSION";
        case NANOCBOR_NOT_FOUND:
            return "NANOCBOR_NOT_FOUND";
        default:
            return "Error value not recognized";
    }
}

/**
 * @brief   Convert nanocbor error to PSA Crypto status value
 *
 * @param   res Nanocbor error value
 * @return  psa_status_t
 */
static psa_status_t nanocbor_error_to_psa_status(int res)
{
    switch (res) {
        case NANOCBOR_ERR_END:
            return PSA_ERROR_BUFFER_TOO_SMALL;
        case NANOCBOR_NOT_FOUND:
            return PSA_ERROR_DOES_NOT_EXIST;
        default:
            return PSA_ERROR_GENERIC_ERROR;
    }
}

/**
 * @brief   Encode a set of key attributes in CBOR
 *
 * @param   enc     Active nanocbor encoder
 * @param   attr    Pointer to key attributes to encode
 * @return  psa_status_t
 */
static psa_status_t psa_encode_key_attributes(nanocbor_encoder_t *enc, psa_key_attributes_t *attr)
{
    int res;

    res = nanocbor_fmt_array(enc, 5);
    if (res < 0) {
        goto error;
    }
    res = nanocbor_fmt_uint(enc, attr->id);
    if (res < 0) {
        goto error;
    }
    res = nanocbor_fmt_uint(enc, attr->type);
    if (res < 0) {
        goto error;
    }
    res = nanocbor_fmt_uint(enc, attr->bits);
    if (res < 0) {
        goto error;
    }
    res = nanocbor_fmt_uint(enc, attr->lifetime);
    if (res < 0) {
        goto error;
    }

    /* Policy */
    res = nanocbor_fmt_array(enc, 2);
    if (res < 0) {
        goto error;
    }
    res = nanocbor_fmt_uint(enc, attr->policy.usage);
    if (res < 0) {
        goto error;
    }
    res = nanocbor_fmt_uint(enc, attr->policy.alg);
    if (res < 0) {
        goto error;
    }

    return PSA_SUCCESS;

    error:
        DEBUG("[persistent storage] error while encoding key attributes: %d = %s", res,
                                                    nanocbor_error_to_string(res));
        return nanocbor_error_to_psa_status(res);
}

#if PSA_PROTECTED_KEY_COUNT
/**
 * @brief   Encode contents of a PSA key slot containing a protected key
 *
 * @param   enc     Active nanocbor encoder
 * @param   slot    Pointer to key slot to encode
 * @return  psa_status_t
 */
static psa_status_t psa_encode_protected_key_slot(nanocbor_encoder_t *enc, psa_key_slot_t *slot)
{
    int res;
    int key_array_size = 1;
    psa_key_slot_number_t *slot_no = psa_key_slot_get_slot_number(slot);

#if IS_USED(MODULE_PSA_ASYMMETRIC)
    if (PSA_KEY_TYPE_IS_KEY_PAIR(slot->attr.type)) {
        key_array_size = 2;
    }
#endif /* MODULE_PSA_ASYMMETRIC */

    res = nanocbor_fmt_array(enc, key_array_size);
    if (res < 0) {
        goto error;
    }

    res = nanocbor_fmt_uint(enc, *slot_no);
    if (res < 0) {
        goto error;
    }

#if IS_USED(MODULE_PSA_ASYMMETRIC)
    if (PSA_KEY_TYPE_IS_KEY_PAIR(slot->attr.type)) {
        uint8_t *pubkey_data;
        size_t *pubkey_data_len;
        psa_get_public_key_data_from_key_slot(slot, &pubkey_data, &pubkey_data_len);

        res = nanocbor_put_bstr(enc, pubkey_data, *pubkey_data_len);
        if (res < 0) {
            goto error;
        }
    }
#endif /* MODULE_PSA_ASYMMETRIC */

    return PSA_SUCCESS;

error:
    DEBUG("[persistent storage] error while encoding protected key: %d = %s", res,
                                                nanocbor_error_to_string(res));
    return nanocbor_error_to_psa_status(res);
}
#endif /* PSA_PROTECTED_KEY_COUNT */

#if PSA_ASYMMETRIC_KEYPAIR_COUNT
/**
 * @brief   Encode contents of a PSA key slot containing an asymmetric key pair
 *
 * @param   enc     Active nanocbor encoder
 * @param   slot    Pointer to key slot to encode
 * @return  psa_status_t
 */
static psa_status_t psa_encode_asymmetric_key_pair(nanocbor_encoder_t *enc, psa_key_slot_t *slot)
{
    int res;
    size_t *privkey_data_len;
    size_t *pubkey_data_len;
    uint8_t *privkey_data;
    uint8_t *pubkey_data;

    psa_get_key_data_from_key_slot(slot, &privkey_data, &privkey_data_len);
    psa_get_public_key_data_from_key_slot(slot, &pubkey_data, &pubkey_data_len);

    res = nanocbor_fmt_array(enc, 2); /* Contains private and public key*/
    if (res < 0) {
        goto error;
    }

    res = nanocbor_put_bstr(enc, privkey_data, *privkey_data_len);
    if (res < 0) {
        goto error;
    }

    res = nanocbor_put_bstr(enc, pubkey_data, *pubkey_data_len);
    if (res < 0) {
        goto error;
    }

    return PSA_SUCCESS;

error:
    DEBUG("[persistent storage] error while encoding asymmetric key: %d = %s", res,
                                                nanocbor_error_to_string(res));
    return nanocbor_error_to_psa_status(res);
}
#endif /* PSA_ASYMMETRIC_KEYPAIR_COUNT */

#if PSA_SINGLE_KEY_COUNT
/**
 * @brief   Encode contents of a PSA key slot containing a single key
 *
 * @param   enc     Active nanocbor encoder
 * @param   slot    Pointer to key slot to encode
 * @return  psa_status_t
 */
static psa_status_t psa_encode_single_key(nanocbor_encoder_t *enc, psa_key_slot_t *slot)
{
    size_t *key_data_len;
    uint8_t *key_data;
    psa_get_key_data_from_key_slot(slot, &key_data, &key_data_len);

    int res = nanocbor_put_bstr(enc, key_data, *key_data_len);
    if (res < 0) {
        DEBUG("[persistent storage] error while encoding single key: %d = %s", res,
                                                nanocbor_error_to_string(res));
        return nanocbor_error_to_psa_status(res);
    }
    return PSA_SUCCESS;
}
#endif /* PSA_SINGLE_KEY_COUNT */

psa_status_t psa_encode_key_slot(psa_key_slot_t *slot, uint8_t *output,
                                     size_t output_len, size_t *output_size)
{
    int res;
    psa_status_t status;
    nanocbor_encoder_t enc;
    nanocbor_encoder_init(&enc, output, output_len);
    *output_size = 0;

    /* Key Struct */
    res = nanocbor_fmt_array(&enc, 2);
    if (res < 0) {
        goto error;
    }

    status = psa_encode_key_attributes(&enc, &slot->attr);
    if (status != PSA_SUCCESS) {
        return status;
    }

#if PSA_PROTECTED_KEY_COUNT
    if (psa_key_lifetime_is_external(slot->attr.lifetime)) {
        status = psa_encode_protected_key_slot(&enc, slot);
        if (status != PSA_SUCCESS) {
            return status;
        }
        goto done;
    }
#endif /* PSA_PROTECTED_KEY_COUNT */

#if PSA_ASYMMETRIC_KEYPAIR_COUNT
    if (PSA_KEY_TYPE_IS_KEY_PAIR(slot->attr.type)) {
        status = psa_encode_asymmetric_key_pair(&enc, slot);
        if (status != PSA_SUCCESS) {
            return status;
        }
        goto done;
    }
#endif /* PSA_ASYMMETRIC_KEYPAIR_COUNT */

#if PSA_SINGLE_KEY_COUNT
    status = psa_encode_single_key(&enc, slot);
    if (status != PSA_SUCCESS) {
        return status;
    }
    goto done;
#else
    return PSA_ERROR_NOT_SUPPORTED;
#endif /* PSA_SINGLE_KEY_COUNT */

done:
    *output_size = nanocbor_encoded_len(&enc);

#if ENABLE_DEBUG
    printf("CBOR Encoded Length: %d\n", *output_size);
    puts("CBOR Encoded Key Slot:\n");
    for (size_t i = 0; i < output_len+1; i++){
        printf("%02x ", output[i]);
    }
    printf("\n");
#endif

    return PSA_SUCCESS;

error:
    DEBUG("[persistent storage] error while encoding key slot: %s",
                                                nanocbor_error_to_string(res));
    return nanocbor_error_to_psa_status(res);
}

psa_status_t psa_decode_key_attributes(psa_key_attributes_t *attr,
                                       uint8_t *cbor_buf, size_t cbor_buf_size)
{
    int res = 0;

    nanocbor_value_t dec;
    nanocbor_decoder_init(&dec, cbor_buf, cbor_buf_size);

    nanocbor_value_t key_slot;
    res = nanocbor_enter_array(&dec, &key_slot);
    if (res < 0) {
        goto error;
    }

    nanocbor_value_t attr_dec;
    res = nanocbor_enter_array(&key_slot, &attr_dec);
    if (res < 0) {
        goto error;
    }
    res = nanocbor_get_uint32(&attr_dec, &attr->id);
    if (res < 0) {
        goto error;
    }
    res = nanocbor_get_uint16(&attr_dec, &attr->type);
    if (res < 0) {
        goto error;
    }
    res = nanocbor_get_uint16(&attr_dec, &attr->bits);
    if (res < 0) {
        goto error;
    }
    res = nanocbor_get_uint32(&attr_dec, &attr->lifetime);
    if (res < 0) {
        goto error;
    }

    nanocbor_value_t policy;
    res = nanocbor_enter_array(&attr_dec, &policy);
    if (res < 0) {
        goto error;
    }
    res = nanocbor_get_uint32(&policy, &attr->policy.usage);
    if (res < 0) {
        goto error;
    }
    res = nanocbor_get_uint32(&policy, &attr->policy.alg);
    if (res < 0) {
        goto error;
    }

#if ENABLE_DEBUG
    puts("Key Attributes:\n");
    printf("ID: %x\n", (int) attr->id);
    printf("Type: %x\n", (int) attr->type);
    printf("Bits: %d\n", (int) attr->bits);
    printf("Lifetime: %x\n", (int) attr->lifetime);
    printf("Usage: %x\n", (int) attr->policy.usage);
    printf("Algorithm: %x\n", (int) attr->policy.alg);
#endif

    return PSA_SUCCESS;

error:
    DEBUG("[persistent storage] error while decoding key attributes: %s",
                                                nanocbor_error_to_string(res));
    return nanocbor_error_to_psa_status(res);
}

/**
 * @brief   Decode CBOR encoded key data
 *
 * @param   key             Nanocbor value object containing CBOR encoding of the key data
 * @param   key_data        Output buffer to write key value
 * @param   key_len         Pointer to write actual key size
 * @return  psa_status_t
 */
static psa_status_t psa_decode_key_data(nanocbor_value_t *key, uint8_t *key_data,
                                        size_t key_data_size, size_t *key_len)
{
    size_t len = 0;
    const uint8_t *buf;
    int res = nanocbor_get_bstr(key, &buf, &len);
    if (res < 0 || len <= 0) {
        key_len = NULL;
        key_data = NULL;
        DEBUG("[persistent storage] error while decoding key: %s",
                                                    nanocbor_error_to_string(res));
        return nanocbor_error_to_psa_status(res);
    }

    if (len > key_data_size) {
        key_len = NULL;
        key_data = NULL;
        DEBUG("[persistent storage] error while decoding key: %s",
                                                    nanocbor_error_to_string(res));
        return PSA_ERROR_CORRUPTION_DETECTED;
    }

    memcpy(key_data, buf, len);
    *key_len = len;

    return PSA_SUCCESS;
}

#if PSA_PROTECTED_KEY_COUNT
/**
 * @brief   Decode CBOR encoded protected key
 *
 * @param   key     Nanocbor value object containing CBOR encoded key data
 * @param   slot    Key slot to write decoded key to
 * @return  psa_status_t
 */
static psa_status_t psa_decode_protected_key_slot(nanocbor_value_t *key, psa_key_slot_t *slot)
{
    int res = 0;

    nanocbor_value_t key_arr;
    res = nanocbor_enter_array(key, &key_arr);
    if (res < 0) {
        goto error;
    }

    psa_key_slot_number_t *slot_no = psa_key_slot_get_slot_number(slot);
    res = nanocbor_get_uint64(&key_arr, slot_no);
    if (res < 0) {
        goto error;
    }

#if IS_USED(MODULE_PSA_ASYMMETRIC)
    if (PSA_KEY_TYPE_IS_KEY_PAIR(slot->attr.type)) {
        size_t *pubkey_data_len;
        uint8_t *pubkey_data;

        psa_get_public_key_data_from_key_slot(slot, &pubkey_data, &pubkey_data_len);

        return psa_decode_key_data(&key_arr, pubkey_data,
                                   PSA_EXPORT_PUBLIC_KEY_MAX_SIZE, pubkey_data_len);
    }
#endif /* MODULE_PSA_ASYMMETRIC */

    return PSA_SUCCESS;

error:
    DEBUG("[persistent storage] error while decoding protected key slot: %s",
                                                nanocbor_error_to_string(res));
    return nanocbor_error_to_psa_status(res);
}
#endif /* PSA_PROTECTED_KEY_COUNT */

#if PSA_ASYMMETRIC_KEYPAIR_COUNT
/**
 * @brief   Decode CBOR encoded asymmetric key pair
 *
 * @param   key     Nanocbor value object containing CBOR encoded key data
 * @param   slot    Key slot to write decoded key to
 * @return  psa_status_t
 */
static psa_status_t psa_decode_asymmetric_keypair_slot(nanocbor_value_t *key, psa_key_slot_t *slot)
{
    int res = 0;
    psa_status_t status;
    size_t *privkey_data_len;
    size_t *pubkey_data_len;
    uint8_t *privkey_data;
    uint8_t *pubkey_data;

    psa_get_key_data_from_key_slot(slot, &privkey_data, &privkey_data_len);
    psa_get_public_key_data_from_key_slot(slot, &pubkey_data, &pubkey_data_len);

    nanocbor_value_t key_arr;
    res = nanocbor_enter_array(key, &key_arr);
    if (res < 0) {
        goto error;
    }

    status = psa_decode_key_data(&key_arr, privkey_data,
                                 PSA_BITS_TO_BYTES(PSA_MAX_PRIV_KEY_SIZE), privkey_data_len);
    if (status != PSA_SUCCESS) {
        return status;
    }

    return psa_decode_key_data(&key_arr, pubkey_data,
                               PSA_EXPORT_PUBLIC_KEY_MAX_SIZE, pubkey_data_len);
error:
    DEBUG("[persistent storage] error while decoding asymmetric key pair: %s",
                                                nanocbor_error_to_string(res));
    return nanocbor_error_to_psa_status(res);
}
#endif /* PSA_ASYMMETRIC_KEYPAIR_COUNT */

psa_status_t psa_decode_key_slot_data(psa_key_slot_t *slot, uint8_t *cbor_buf, size_t cbor_buf_size)
{
    int res = 0;
    psa_status_t status = PSA_ERROR_GENERIC_ERROR;

    nanocbor_value_t dec;
    nanocbor_decoder_init(&dec, cbor_buf, cbor_buf_size);

    nanocbor_value_t key_slot;
    res = nanocbor_enter_array(&dec, &key_slot);
    if (res < 0) {
        goto error;
    }

    /* We should have decoded the attributes before, so we can skip them here */
    nanocbor_skip(&key_slot);

#if PSA_PROTECTED_KEY_COUNT
    if (psa_key_lifetime_is_external(slot->attr.lifetime)) {
        return psa_decode_protected_key_slot(&key_slot, slot);
    }
#endif /* PSA_PROTECTED_KEY_COUNT */

#if PSA_ASYMMETRIC_KEYPAIR_COUNT
    if (PSA_KEY_TYPE_IS_KEY_PAIR(slot->attr.type)) {
        return psa_decode_asymmetric_keypair_slot(&key_slot, slot);
    }
#endif /* PSA_ASYMMETRIC_KEYPAIR_COUNT */

#if PSA_SINGLE_KEY_COUNT
    size_t *key_data_len;
    uint8_t *key_data;

    psa_get_key_data_from_key_slot(slot, &key_data, &key_data_len);

    return psa_decode_key_data(&key_slot, key_data, PSA_MAX_KEY_DATA_SIZE, key_data_len);
#endif /* PSA_SINGLE_KEY_COUNT */

    return status;

error:
    DEBUG("[persistent storage] error while decoding key slot: %s",
                                                nanocbor_error_to_string(res));
    return nanocbor_error_to_psa_status(res);
}
