#include "test_utils.h"

static void test_psa_generate_sign_verify_ecc_p256_key(void)
{
    psa_key_id_t privkey_id;
    psa_key_attributes_t privkey_attr = psa_key_attributes_init();
    psa_key_id_t pubkey_id;
    psa_key_attributes_t pubkey_attr = psa_key_attributes_init();

    psa_key_lifetime_t lifetime = PSA_KEY_LIFETIME_FROM_PERSISTENCE_AND_LOCATION(
        PSA_KEY_LIFETIME_VOLATILE, PSA_KEY_LOCATION_LOCAL_SEALED);
    psa_key_usage_t usage = PSA_KEY_USAGE_SIGN_HASH | PSA_KEY_USAGE_VERIFY_HASH;

    uint8_t public_key[PSA_EXPORT_PUBLIC_KEY_OUTPUT_SIZE(ECC_KEY_TYPE, ECC_KEY_SIZE)] = { 0 };
    size_t pubkey_length;
    uint8_t signature[PSA_SIGN_OUTPUT_SIZE(ECC_KEY_TYPE, ECC_KEY_SIZE, ECC_ALG)];
    size_t sig_length;
    uint8_t msg[ECDSA_MESSAGE_SIZE] = { 0x0b };
    uint8_t hash[PSA_HASH_LENGTH(ECC_ALG_HASH)];
    size_t hash_length;

    psa_set_key_algorithm(&privkey_attr, ECC_ALG);
    psa_set_key_usage_flags(&privkey_attr, usage);
    psa_set_key_type(&privkey_attr, ECC_KEY_TYPE);
    psa_set_key_bits(&privkey_attr, ECC_KEY_SIZE);
    psa_set_key_lifetime(&privkey_attr, lifetime);

    TEST_ASSERT_PSA_SUCCESS(psa_generate_key(&privkey_attr, &privkey_id));

    TEST_ASSERT_PSA_SUCCESS(psa_export_public_key(privkey_id, public_key, sizeof(public_key), &pubkey_length));

    TEST_ASSERT_PSA_SUCCESS(psa_hash_compute(ECC_ALG_HASH, msg, sizeof(msg), hash, sizeof(hash), &hash_length));

    psa_set_key_usage_flags(&pubkey_attr, PSA_KEY_USAGE_VERIFY_MESSAGE);
    psa_set_key_algorithm(&pubkey_attr, ECC_ALG);
    psa_set_key_bits(&pubkey_attr, ECC_KEY_SIZE);
    psa_set_key_type(&pubkey_attr, PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_SECP_R1));

    TEST_ASSERT_PSA_SUCCESS(psa_import_key(&pubkey_attr, public_key, pubkey_length, &pubkey_id));

    TEST_ASSERT_PSA_SUCCESS(psa_sign_hash(privkey_id, ECC_ALG, hash, sizeof(hash), signature, sizeof(signature), &sig_length));

    /* verify on original message with internal hashing operation */
    TEST_ASSERT_PSA_SUCCESS(psa_verify_message(pubkey_id, ECC_ALG, msg, sizeof(msg), signature, sig_length));

    psa_destroy_key(privkey_id);
    psa_destroy_key(pubkey_id);
}

Test* tests_psa_generate_sealed_key(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_psa_generate_sign_verify_ecc_p256_key),
    };

    EMB_UNIT_TESTCALLER(tests_psa_generate_key_tests, NULL, NULL, fixtures);

    return (Test *)&tests_psa_generate_key_tests;
}
