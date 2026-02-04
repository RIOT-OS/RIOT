/*
 * SPDX-FileCopyrightText: 2014 Philipp Rosenkranz
 * SPDX-FileCopyrightText: 2014 Nico von Geyso
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#include "tests-crypto.h"

int main(void)
{
    TESTS_START();
    TESTS_RUN(tests_crypto_helper_tests());
    TESTS_RUN(tests_crypto_chacha_tests());
    TESTS_RUN(tests_crypto_poly1305_tests());
    TESTS_RUN(tests_crypto_chacha20poly1305_tests());
    TESTS_RUN(tests_crypto_aes_tests());
    TESTS_RUN(tests_crypto_cipher_tests());
    TESTS_RUN(tests_crypto_modes_ccm_tests());
    TESTS_RUN(tests_crypto_modes_ocb_tests());
    TESTS_RUN(tests_crypto_modes_ecb_tests());
    TESTS_RUN(tests_crypto_modes_cbc_tests());
    TESTS_RUN(tests_crypto_modes_ctr_tests());
    TESTS_END();
    return 0;
}
