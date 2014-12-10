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
    TESTS_RUN(tests_crypto_sha256_tests());
}
