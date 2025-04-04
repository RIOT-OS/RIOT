/*
 * Copyright (C) 2023 HAW Hamburg
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
 * @brief       Test application for the PSA Crypto key encoding module
 *
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 * @}
 */

#include "psa/crypto.h"
#include "tests_psa_persistent_storage.h"

int main(void)
{
    TESTS_START();
    TESTS_RUN(tests_psa_persistent_single_key_storage());
    TESTS_RUN(tests_psa_persistent_asym_keypair_storage());
    TESTS_RUN(tests_psa_fail_overwrite_existing_key());
    TESTS_END();
    return 0;
}
