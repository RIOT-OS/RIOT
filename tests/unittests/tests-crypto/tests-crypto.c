/*
 * Copyright (C) 2014 Philipp Rosenkranz
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include "tests-crypto.h"

void tests_crypto(void)
{
    TESTS_RUN(tests_crypto_dragon_tests());
    TESTS_RUN(tests_crypto_hc128_tests());
    TESTS_RUN(tests_crypto_hc256_tests());
    TESTS_RUN(tests_crypto_lexv2_tests());
    TESTS_RUN(tests_crypto_nlsv2_tests());
    TESTS_RUN(tests_crypto_rabbit_tests());
    TESTS_RUN(tests_crypto_salsa20_tests());
    TESTS_RUN(tests_crypto_sha256_tests());
    TESTS_RUN(tests_crypto_sosemanuk_tests());
}
