/*
 * Copyright (C) 2021 Nils Ollrogge
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#include "tests-crypto.h"

int main(void)
{
    TESTS_START();
    TESTS_RUN(tests_crypto_modes_ccm_tests_128());
    TESTS_RUN(tests_crypto_modes_ccm_tests_192());
    TESTS_RUN(tests_crypto_modes_ccm_tests_256());
    TESTS_END();
    return 0;
}
