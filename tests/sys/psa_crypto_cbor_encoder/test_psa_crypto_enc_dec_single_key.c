/*
 * Copyright (C) 2023 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Tests CBOR encoding of a PSA Crypto key slot containing one single key
 *
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 * @}
 */

#include "embUnit.h"
#include "psa/crypto.h"
#include "psa_crypto_cbor_encoder.h"
#include "tests_psa_cbor_enc_dec.h"

/**
 * @brief   Data has been obtained by manually encoding and decoding keys,
 *          until they fit the desired format and checking the output with
 *          https://cbor.me.
 *
 *          Diagnostic notation:
 *          [
 *              [268435459, 9216, 128, 1, [256, 71319552]],
 *              h'2B7E151628AED218ABF7158809CF4F3C'
 *          ]
 */
static uint8_t cbor_encoded_data[] = {
    0x82, 0x85, 0x1a, 0x10, 0x00, 0x00, 0x03, 0x19,
    0x24, 0x00, 0x18, 0x80, 0x01, 0x82, 0x19, 0x01,
    0x00, 0x1a, 0x04, 0x40, 0x40, 0x00, 0x50, 0x2b,
    0x7e, 0x15, 0x16, 0x28, 0xae, 0xd2, 0x18, 0xab,
    0xf7, 0x15, 0x88, 0x09, 0xcf, 0x4f, 0x3c
};

static void _init_key_slot(psa_key_slot_t *slot)
{
    psa_set_key_type(&slot->attr, PSA_KEY_TYPE_AES);
    psa_set_key_algorithm(&slot->attr, PSA_ALG_CBC_NO_PADDING);
    psa_set_key_bits(&slot->attr, PSA_BYTES_TO_BITS(sizeof(AES_KEY)));
    psa_set_key_lifetime(&slot->attr, 0x00000001);
    psa_set_key_usage_flags(&slot->attr, PSA_KEY_USAGE_ENCRYPT);
    psa_set_key_id(&slot->attr, 0x10000003);

    memcpy(slot->key.data, AES_KEY, sizeof(AES_KEY));
    slot->key.data_len = sizeof(AES_KEY);
}

/**
 * @brief   CBOR encoding of key pair slot should equal @c cbor_encoded_data
 */
static void test_encode_single_key_slot(void)
{
    size_t encoded_size;
    uint8_t cbor_enc[sizeof(cbor_encoded_data)];
    psa_key_slot_t slot;
    _init_key_slot(&slot);

    TEST_ASSERT_PSA(psa_encode_key_slot(&slot, cbor_enc, sizeof(cbor_enc), &encoded_size));
    TEST_ASSERT_EQUAL_INT(sizeof(cbor_encoded_data), encoded_size);
    TEST_ASSERT_MESSAGE(0 == memcmp(cbor_enc, cbor_encoded_data,
                                     encoded_size), "wrong cbor encoding");
}

/**
 * @brief   Decoded key slot should equal key slot structure initialized
 *          in @c test_encode_single_key_slot.
 */
static void test_decode_single_key_slot(void)
{
    psa_key_slot_t slot;

    TEST_ASSERT_PSA(psa_decode_key_attributes(&(slot.attr), cbor_encoded_data, sizeof(cbor_encoded_data)));
    TEST_ASSERT_PSA(psa_decode_key_slot_data(&slot, cbor_encoded_data, sizeof(cbor_encoded_data)));
    TEST_ASSERT_MESSAGE(0 == memcmp((uint8_t *)&slot, (uint8_t *)&slot,
                                     sizeof(psa_key_slot_t)), "wrong cbor decoding");
}

Test* tests_psa_crypto_enc_dec_single_key(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_encode_single_key_slot),
        new_TestFixture(test_decode_single_key_slot),
    };

    EMB_UNIT_TESTCALLER(psa_crypto_enc_dec_single_key_tests, NULL, NULL, fixtures);

    return (Test *)&psa_crypto_enc_dec_single_key_tests;
}
