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

#define TEST_ASSERT_PSA(func_) TEST_ASSERT_MESSAGE(func_ == PSA_SUCCESS, #func_ " failed");

/*
 * A second call to psa_crypto_init() should not reset key data.
 */
static void test_init_twice(void)
{
    TEST_ASSERT_PSA(psa_crypto_init());


    psa_key_id_t key_id = PSA_KEY_ID_NULL;
    psa_key_attributes_t key_attr = psa_key_attributes_init();
    psa_set_key_algorithm(&key_attr, PSA_ALG_PURE_EDDSA);
    psa_set_key_usage_flags(&key_attr, PSA_KEY_USAGE_EXPORT);
    psa_set_key_bits(&key_attr, 255);
    psa_set_key_type(&key_attr, PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_TWISTED_EDWARDS));
    TEST_ASSERT_PSA(psa_generate_key(&key_attr, &key_id));

    uint8_t key_data[PSA_EXPORT_PUBLIC_KEY_OUTPUT_SIZE(psa_get_key_type(&key_attr), psa_get_key_bits(&key_attr))];
    size_t key_data_len;

    TEST_ASSERT_PSA(psa_export_public_key(key_id, key_data, sizeof(key_data), &key_data_len));
    TEST_ASSERT_PSA(psa_crypto_init());
    TEST_ASSERT_PSA(psa_export_public_key(key_id, key_data, sizeof(key_data), &key_data_len));

}

static Test *tests_psa_crypto(void)
{
    EMB_UNIT_TESTFIXTURES(fixtures) {
        new_TestFixture(test_init_twice),
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
