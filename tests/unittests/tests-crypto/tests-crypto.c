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
    TESTS_RUN(tests_crypto_c25519_tests());
    TESTS_RUN(tests_crypto_ed25519_tests());
    TESTS_RUN(tests_crypto_edsign_tests());
    TESTS_RUN(tests_crypto_morph25519_tests());
    TESTS_RUN(tests_crypto_sha256_tests());
    TESTS_RUN(tests_crypto_sha512_tests());
}
