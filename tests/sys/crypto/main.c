/*
 * Copyright (C) 2014 Philipp Rosenkranz
 * Copyright (C) 2014 Nico von Geyso
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
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
