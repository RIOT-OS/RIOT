/*
 * SPDX-FileCopyrightText: 2021 Nils Ollrogge
 * SPDX-License-Identifier: LGPL-2.1-only
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
