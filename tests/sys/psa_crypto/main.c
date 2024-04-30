/*
 * Copyright (C) 2023 TU Dresden
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
 * @brief       Test application for the PSA Cryptography API
 *
 * @author      Mikolai Gütschow <mikolai.guetschow@tu-dresden.de>
 *
 * @}
 */

#include <stdio.h>
#include "embUnit.h"
#include "psa/crypto.h"

void addFailurePSA(const char *func, psa_status_t errcode, long line, const char *file)
{
    static char msg[128];
    strncpy(msg, func, sizeof(msg));
    strcat(msg, ": ");
    strcat(msg, psa_status_to_humanly_readable(errcode));
    addFailure(msg, line, file);
}

#define TEST_ASSERT_PSA(func_, do_) { psa_status_t ret = func_; if (ret != PSA_SUCCESS) { addFailurePSA(#func_, ret, __LINE__, __FILE__); do_; } }
#define TEST_ASSERT_PSA_CLEANUP(func_) TEST_ASSERT_PSA(func_, goto cleanup)
#define TEST_ASSERT_PSA_RETURN(func_) TEST_ASSERT_PSA(func_, return)
#define TEST_ASSERT_PSA_CONTINUE(func_) TEST_ASSERT_PSA(func_, )

/*
 * A second call to psa_crypto_init() should not reset key data.
 */
static void test_init_twice(void)
{
    const psa_key_type_t key_type = PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_TWISTED_EDWARDS);
    const size_t key_bits = 255;
    const psa_algorithm_t key_alg = PSA_ALG_PURE_EDDSA;

    uint8_t key_data[PSA_EXPORT_PUBLIC_KEY_OUTPUT_SIZE(key_type, key_bits)];
    size_t key_data_len;

    psa_key_id_t key_id = PSA_KEY_ID_NULL;
    psa_key_attributes_t key_attr = psa_key_attributes_init();
    psa_set_key_algorithm(&key_attr, key_alg);
    psa_set_key_usage_flags(&key_attr, PSA_KEY_USAGE_EXPORT);
    psa_set_key_bits(&key_attr, key_bits);
    psa_set_key_type(&key_attr, key_type);

    TEST_ASSERT_PSA_CLEANUP(psa_crypto_init());
    TEST_ASSERT_PSA_CLEANUP(psa_generate_key(&key_attr, &key_id));

    TEST_ASSERT_PSA_CLEANUP(psa_export_public_key(key_id, key_data, sizeof(key_data), &key_data_len));
    TEST_ASSERT_PSA_CLEANUP(psa_crypto_init());
    TEST_ASSERT_PSA_CLEANUP(psa_export_public_key(key_id, key_data, sizeof(key_data), &key_data_len));

cleanup:
    TEST_ASSERT_PSA_CONTINUE(psa_destroy_key(key_id));
}

/**
 * Exporting and re-importing a private Ed25519 key should result in the same public key and signature.
 */
static void test_exported_key_is_identical_when_imported_again_ed25519(void)
{
    const psa_key_type_t key_type = PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_TWISTED_EDWARDS);
    const size_t key_bits = 255;
    const psa_algorithm_t key_alg = PSA_ALG_PURE_EDDSA;

    // buffers to hold randomly generated Ed25519 keypair
    uint8_t privkey[PSA_EXPORT_KEY_OUTPUT_SIZE(key_type, key_bits)];
    size_t privkey_len;
    uint8_t pubkey[PSA_EXPORT_PUBLIC_KEY_OUTPUT_SIZE(key_type, key_bits)];
    size_t pubkey_len;

    // buffer to hold Ed25519 public key derived from imported private key
    uint8_t pubkey2[PSA_EXPORT_PUBLIC_KEY_OUTPUT_SIZE(key_type, key_bits)];
    size_t pubkey2_len;

    // buffers to hold signature for verification
    uint8_t msg[] = "Hello world!";
    uint8_t sig[PSA_SIGN_OUTPUT_SIZE(key_type, key_bits, key_alg)];
    size_t sig_len;
    uint8_t sig2[PSA_SIGN_OUTPUT_SIZE(key_type, key_bits, key_alg)];
    size_t sig2_len;

    // work on Ed25519 keypair which can be exported and used for signing messages
    psa_key_id_t key_id = PSA_KEY_ID_NULL;
    psa_key_attributes_t key_attr = psa_key_attributes_init();
    psa_set_key_algorithm(&key_attr, key_alg);
    psa_set_key_usage_flags(&key_attr, PSA_KEY_USAGE_SIGN_MESSAGE | PSA_KEY_USAGE_EXPORT);
    psa_set_key_bits(&key_attr, key_bits);
    psa_set_key_type(&key_attr, key_type);

    // randomly generate keypair
    TEST_ASSERT_PSA_CLEANUP(psa_crypto_init());
    TEST_ASSERT_PSA_CLEANUP(psa_generate_key(&key_attr, &key_id));

    // sign msg with generated keypair
    TEST_ASSERT_PSA_CLEANUP(psa_sign_message(key_id, key_alg, msg, sizeof(msg), sig, sizeof(sig), &sig_len));

    // export public and private key, then free slot
    TEST_ASSERT_PSA_CLEANUP(psa_export_public_key(key_id, pubkey, sizeof(pubkey), &pubkey_len));
    TEST_ASSERT_PSA_CLEANUP(psa_export_key(key_id, privkey, sizeof(privkey), &privkey_len));
    TEST_ASSERT_PSA_CLEANUP(psa_destroy_key(key_id));

    // import private key and compare public keys
    psa_set_key_usage_flags(&key_attr, PSA_KEY_USAGE_SIGN_MESSAGE);
    TEST_ASSERT_PSA_CLEANUP(psa_import_key(&key_attr, privkey, privkey_len, &key_id));
    TEST_ASSERT_PSA_CLEANUP(psa_export_public_key(key_id, pubkey2, sizeof(pubkey2), &pubkey2_len));
    TEST_ASSERT(pubkey_len == pubkey2_len && memcmp(pubkey, pubkey2, pubkey_len) == 0);

    // sign msg with imported key and compare signatures
    TEST_ASSERT_PSA_CLEANUP(psa_sign_message(key_id, key_alg, msg, sizeof(msg), sig2, sizeof(sig2), &sig2_len));
    TEST_ASSERT(sig_len == sig2_len && memcmp(sig, sig2, sig_len) == 0);

cleanup:
    TEST_ASSERT_PSA_CONTINUE(psa_destroy_key(key_id));
}

/**
 * psa_export_key() is an alias for psa_export_public_key() if the given key is a public key
 */
static void test_export_public_key_ed25519(void)
{
    const psa_key_type_t key_type = PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_TWISTED_EDWARDS);
    const psa_key_type_t key_type2 = PSA_KEY_TYPE_ECC_PUBLIC_KEY(PSA_ECC_FAMILY_TWISTED_EDWARDS);
    const size_t key_bits = 255;
    const psa_algorithm_t key_alg = PSA_ALG_PURE_EDDSA;

    // buffers to hold exported public key
    uint8_t pubkey[PSA_EXPORT_PUBLIC_KEY_OUTPUT_SIZE(key_type, key_bits)];
    size_t pubkey_len;
    uint8_t pubkey2[PSA_EXPORT_KEY_OUTPUT_SIZE(key_type2, key_bits)];
    size_t pubkey2_len;

    // work on Ed25519 keypair which has no special usage permissions
    // as exporting the public key is allowed even without PSA_KEY_USAGE_EXPORT
    psa_key_id_t key_id = PSA_KEY_ID_NULL;
    psa_key_attributes_t key_attr = psa_key_attributes_init();
    psa_set_key_algorithm(&key_attr, key_alg);
    psa_set_key_usage_flags(&key_attr, 0);
    psa_set_key_bits(&key_attr, key_bits);
    psa_set_key_type(&key_attr, key_type);

    psa_key_id_t key_id2 = PSA_KEY_ID_NULL;

    // randomly generate keypair
    TEST_ASSERT_PSA_CLEANUP(psa_crypto_init());
    TEST_ASSERT_PSA_CLEANUP(psa_generate_key(&key_attr, &key_id));

    // export public key
    TEST_ASSERT_PSA_CLEANUP(psa_export_public_key(key_id, pubkey, sizeof(pubkey), &pubkey_len));

    // import public key into single-key slot
    psa_set_key_type(&key_attr, key_type2);
    TEST_ASSERT_PSA_CLEANUP(psa_import_key(&key_attr, pubkey, pubkey_len, &key_id2));

    // export public key from single slot using `psa_export_key()` and compare
    TEST_ASSERT_PSA_CLEANUP(psa_export_key(key_id2, pubkey2, sizeof(pubkey2), &pubkey2_len));
    TEST_ASSERT(pubkey_len == pubkey2_len && memcmp(pubkey, pubkey2, pubkey_len) == 0);

    // export public key from single slot using `psa_export_public_key()` and compare again
    TEST_ASSERT_PSA_CLEANUP(psa_export_public_key(key_id2, pubkey2, sizeof(pubkey2), &pubkey2_len));
    TEST_ASSERT(pubkey_len == pubkey2_len && memcmp(pubkey, pubkey2, pubkey_len) == 0);

cleanup:
    TEST_ASSERT_PSA_CONTINUE(psa_destroy_key(key_id));
    TEST_ASSERT_PSA_CONTINUE(psa_destroy_key(key_id2));
}

static Test *tests_psa_crypto(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_init_twice),
        new_TestFixture(test_exported_key_is_identical_when_imported_again_ed25519),
        new_TestFixture(test_export_public_key_ed25519),
    };

    EMB_UNIT_TESTCALLER(tests, NULL, NULL, fixtures);
    return (Test *)&tests;
}

int main(void)
{
    puts("psa_crypto test");
    TESTS_START();
    TESTS_RUN(tests_psa_crypto());
    TESTS_END();

    return 0;
}
