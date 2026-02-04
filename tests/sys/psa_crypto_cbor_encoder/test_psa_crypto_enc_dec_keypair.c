/*
 * SPDX-FileCopyrightText: 2023 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Tests CBOR encoding of a PSA Crypto asymmetric key pair slot.
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
 *              [268435459, 28946, 256, 1, [12288, 100664841]],
 *              [h'27FC4D41F3DE49F786314B74AA67DE4BA961C38F4F896F045A537102B43D2039',
 *               h'0456CB81D1CBDE44F51DCCB12602670D76DDE784ED8D30721CCA5059F920AD62
 *               87749EC9CB2675C51B69A68956102E8F6F7257B9B993ED8899EAFD53823DCAB641']
 *          ]
 */
static uint8_t cbor_encoded_data[] = {
    0x82, 0x85, 0x1a, 0x10, 0x00, 0x00, 0x03, 0x19,
    0x71, 0x12, 0x19, 0x01, 0x00, 0x01, 0x82, 0x19,
    0x30, 0x00, 0x1a, 0x06, 0x00, 0x06, 0x09, 0x82,
    0x58, 0x20, 0x27, 0xfc, 0x4d, 0x41, 0xf3, 0xde,
    0x49, 0xf7, 0x86, 0x31, 0x4b, 0x74, 0xaa, 0x67,
    0xde, 0x4b, 0xa9, 0x61, 0xc3, 0x8f, 0x4f, 0x89,
    0x6f, 0x04, 0x5a, 0x53, 0x71, 0x02, 0xb4, 0x3d,
    0x20, 0x39, 0x58, 0x41, 0x04, 0x56, 0xcb, 0x81,
    0xd1, 0xcb, 0xde, 0x44, 0xf5, 0x1d, 0xcc, 0xb1,
    0x26, 0x02, 0x67, 0x0d, 0x76, 0xdd, 0xe7, 0x84,
    0xed, 0x8d, 0x30, 0x72, 0x1c, 0xca, 0x50, 0x59,
    0xf9, 0x20, 0xad, 0x62, 0x87, 0x74, 0x9e, 0xc9,
    0xcb, 0x26, 0x75, 0xc5, 0x1b, 0x69, 0xa6, 0x89,
    0x56, 0x10, 0x2e, 0x8f, 0x6f, 0x72, 0x57, 0xb9,
    0xb9, 0x93, 0xed, 0x88, 0x99, 0xea, 0xfd, 0x53,
    0x82, 0x3d, 0xca, 0xb6, 0x41
};

static void _init_key_slot(psa_key_pair_slot_t *slot)
{
    psa_key_lifetime_t lifetime = 1;
    psa_key_usage_t usage = PSA_KEY_USAGE_SIGN_HASH | PSA_KEY_USAGE_VERIFY_HASH;

    psa_set_key_lifetime(&slot->attr, lifetime);
    psa_set_key_algorithm(&slot->attr, ECC_ALG);
    psa_set_key_usage_flags(&slot->attr, usage);
    psa_set_key_type(&slot->attr, ECC_KEY_TYPE);
    psa_set_key_bits(&slot->attr, ECC_KEY_SIZE);
    psa_set_key_id(&slot->attr, 0x10000003);

    memcpy(slot->key.privkey_data, privkey, sizeof(privkey));
    slot->key.privkey_data_len = sizeof(privkey);

    memcpy(slot->key.pubkey_data, pubkey, sizeof(pubkey));
    slot->key.pubkey_data_len = sizeof(pubkey);
}

/**
 * @brief   CBOR encoding of key pair slot should equal @c cbor_encoded_data
 */
static void test_encode_asymmetric_keypair_slot(void)
{
    size_t encoded_size;
    uint8_t cbor_enc[sizeof(cbor_encoded_data)];
    psa_key_pair_slot_t slot;
    _init_key_slot(&slot);

    TEST_ASSERT_PSA(psa_encode_key_slot((psa_key_slot_t *)&slot, cbor_enc, sizeof(cbor_enc), &encoded_size));
    TEST_ASSERT_EQUAL_INT(sizeof(cbor_encoded_data), encoded_size);
    TEST_ASSERT_MESSAGE(0 == memcmp(cbor_enc, cbor_encoded_data,
                                     encoded_size), "wrong cbor encoding");
}

/**
 * @brief   Decoded key slot should equal key slot structure initialized
 *          in @c test_encode_asymmetric_keypair_slot.
 */
static void test_decode_asymmetric_keypair_slot(void)
{
    psa_key_pair_slot_t slot;

    TEST_ASSERT_PSA(psa_decode_key_attributes(&slot.attr, cbor_encoded_data, sizeof(cbor_encoded_data)));
    TEST_ASSERT_PSA(psa_decode_key_slot_data((psa_key_slot_t *)&slot, cbor_encoded_data, sizeof(cbor_encoded_data)));
    TEST_ASSERT_MESSAGE(0 == memcmp((uint8_t *)&slot, (uint8_t *)&slot,
                                     sizeof(psa_key_pair_slot_t)), "wrong cbor decoding");
}

Test* tests_psa_crypto_enc_dec_keypair(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_encode_asymmetric_keypair_slot),
        new_TestFixture(test_decode_asymmetric_keypair_slot),
    };

    EMB_UNIT_TESTCALLER(psa_crypto_enc_dec_keypair_tests, NULL, NULL, fixtures);

    return (Test *)&psa_crypto_enc_dec_keypair_tests;
}
