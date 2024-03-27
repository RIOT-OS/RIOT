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
 * @brief       Tests CBOR encoding of PSA Crypto protected key slots
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
 * @brief   Testdata has been obtained by manually encoding and decoding keys,
 *          until they fit the desired format and checking the output with
 *          https://cbor.me.
 *
 *          Diagnostic notation:
 *          [
 *              [8, 9216, 128, 2147483648, [768, 71319552]],
 *              [4]
 *          ]
 */
static uint8_t cbor_encoded_data[] = {
    0x82, 0x85, 0x08, 0x19, 0x24, 0x00, 0x18, 0x80,
    0x1a, 0x80, 0x00, 0x00, 0x00, 0x82, 0x19, 0x03,
    0x00, 0x1a, 0x04, 0x40, 0x40, 0x00, 0x81, 0x04
};

/**
 * @brief   Testdata has been obtained by manually encoding and decoding keys,
 *          until they fit the desired format and checking the output with
 *          https://cbor.me.
 *
 *          Diagnostic notation:
 *          [
 *              [8, 28946, 256, 2147483648, [12288, 100664841]],
 *              [4, h'0456CB81D1CBDE44F51DCCB12602670D76DDE784ED8D30721CCA5059F920AD62877
 *                    49EC9CB2675C51B69A68956102E8F6F7257B9B993ED8899EAFD53823DCAB641']
 *          ]
 */
static uint8_t cbor_encoded_data_with_pubkey[] = {
    0x82, 0x85, 0x08, 0x19, 0x71, 0x12, 0x19, 0x01,
    0x00, 0x1a, 0x80, 0x00, 0x00, 0x00, 0x82, 0x19,
    0x30, 0x00, 0x1a, 0x06, 0x00, 0x06, 0x09, 0x82,
    0x04, 0x58, 0x41, 0x04, 0x56, 0xcb, 0x81, 0xd1,
    0xcb, 0xde, 0x44, 0xf5, 0x1d, 0xcc, 0xb1, 0x26,
    0x02, 0x67, 0x0d, 0x76, 0xdd, 0xe7, 0x84, 0xed,
    0x8d, 0x30, 0x72, 0x1c, 0xca, 0x50, 0x59, 0xf9,
    0x20, 0xad, 0x62, 0x87, 0x74, 0x9e, 0xc9, 0xcb,
    0x26, 0x75, 0xc5, 0x1b, 0x69, 0xa6, 0x89, 0x56,
    0x10, 0x2e, 0x8f, 0x6f, 0x72, 0x57, 0xb9, 0xb9,
    0x93, 0xed, 0x88, 0x99, 0xea, 0xfd, 0x53, 0x82,
    0x3d, 0xca, 0xb6, 0x41
};

static void _init_key_slot(psa_prot_key_slot_t *slot, int is_pubkey)
{
    psa_key_lifetime_t lifetime = PSA_KEY_LIFETIME_FROM_PERSISTENCE_AND_LOCATION
                                      (PSA_KEY_LIFETIME_VOLATILE, PSA_KEY_LOCATION_SE_MIN);
    psa_set_key_lifetime(&slot->attr, lifetime);
    psa_set_key_id(&slot->attr, 8);
    slot->key.slot_number = 4;

    if (!is_pubkey) {
        psa_key_usage_t usage = PSA_KEY_USAGE_ENCRYPT | PSA_KEY_USAGE_DECRYPT;
        psa_set_key_algorithm(&slot->attr, PSA_ALG_CBC_NO_PADDING);
        psa_set_key_usage_flags(&slot->attr, usage);
        psa_set_key_type(&slot->attr, PSA_KEY_TYPE_AES);
        psa_set_key_bits(&slot->attr, PSA_BYTES_TO_BITS(sizeof(AES_KEY)));
    }
    else {
        psa_key_usage_t usage = PSA_KEY_USAGE_SIGN_HASH | PSA_KEY_USAGE_VERIFY_HASH;
        psa_set_key_algorithm(&slot->attr, ECC_ALG);
        psa_set_key_usage_flags(&slot->attr, usage);
        psa_set_key_type(&slot->attr, ECC_KEY_TYPE);
        psa_set_key_bits(&slot->attr, ECC_KEY_SIZE);
        psa_set_key_id(&slot->attr, 8);

        memcpy(slot->key.pubkey_data, pubkey, sizeof(pubkey));
        slot->key.pubkey_data_len = sizeof(pubkey);
    }
}

/**
 * @brief   CBOR encoding of key pair slot should equal @c cbor_encoded_data
 */
static void test_encode_protected_key_slot(void)
{
    size_t encoded_size;
    uint8_t cbor_enc[sizeof(cbor_encoded_data)];
    psa_prot_key_slot_t slot;
    _init_key_slot(&slot, 0);

    TEST_ASSERT_PSA(psa_encode_key_slot((psa_key_slot_t *)&slot, cbor_enc, sizeof(cbor_enc), &encoded_size));
    TEST_ASSERT_EQUAL_INT(sizeof(cbor_encoded_data), encoded_size);
    TEST_ASSERT_MESSAGE(0 == memcmp(cbor_enc, cbor_encoded_data, encoded_size),
                                                                "wrong cbor encoding");
}

/**
 * @brief   CBOR encoding of key pair slot should equal @c cbor_encoded_data_with_pubkey
 */
static void test_encode_protected_key_slot_with_pubkey(void)
{
    size_t encoded_size;
    uint8_t cbor_enc_with_pubkey[sizeof(cbor_encoded_data_with_pubkey)];
    psa_prot_key_slot_t slot;
    _init_key_slot(&slot, 1);

    TEST_ASSERT_PSA(psa_encode_key_slot((psa_key_slot_t *)&slot, cbor_enc_with_pubkey, sizeof(cbor_enc_with_pubkey), &encoded_size));
    TEST_ASSERT_EQUAL_INT(sizeof(cbor_encoded_data_with_pubkey), encoded_size);
    TEST_ASSERT_MESSAGE(0 == memcmp(cbor_enc_with_pubkey, cbor_encoded_data_with_pubkey,
                                     encoded_size), "wrong cbor encoding");
}

/**
 * @brief   Decoded key slot should equal key slot structure initialized
 *          in @c test_encode_protected_key_slot.
 */
static void test_decode_protected_key_slot(void)
{
    psa_prot_key_slot_t slot;

    TEST_ASSERT_PSA(psa_decode_key_attributes(&slot.attr, cbor_encoded_data, sizeof(cbor_encoded_data)));
    TEST_ASSERT_PSA(psa_decode_key_slot_data((psa_key_slot_t *)&slot, cbor_encoded_data, sizeof(cbor_encoded_data)));
    TEST_ASSERT_MESSAGE(0 == memcmp((uint8_t *)&slot, (uint8_t *)&slot,
                                     sizeof(psa_prot_key_slot_t)), "wrong cbor decoding");
}

/**
 * @brief   Decoded key slot should equal key slot structure initialized
 *          in @c test_encode_protected_key_slot_with_pubkey.
 */
static void test_decode_protected_key_slot_with_pubkey(void)
{
    psa_prot_key_slot_t slot;

    TEST_ASSERT_PSA(psa_decode_key_attributes(&slot.attr, cbor_encoded_data_with_pubkey, sizeof(cbor_encoded_data_with_pubkey)));
    TEST_ASSERT_PSA(psa_decode_key_slot_data((psa_key_slot_t *)&slot, cbor_encoded_data_with_pubkey, sizeof(cbor_encoded_data_with_pubkey)));

    TEST_ASSERT_MESSAGE(0 == memcmp((uint8_t *)&slot, (uint8_t *)&slot,
                                     sizeof(psa_prot_key_slot_t)), "wrong cbor decoding");
}

Test* tests_psa_crypto_enc_dec_protected_key(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_encode_protected_key_slot),
        new_TestFixture(test_decode_protected_key_slot),
        new_TestFixture(test_encode_protected_key_slot_with_pubkey),
        new_TestFixture(test_decode_protected_key_slot_with_pubkey)
    };

    EMB_UNIT_TESTCALLER(psa_crypto_enc_dec_protected_key_tests, NULL, NULL, fixtures);

    return (Test *)&psa_crypto_enc_dec_protected_key_tests;
}
