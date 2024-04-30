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

#include "tests_psa_cbor_enc_dec.h"

int main(void)
{
    TESTS_START();
    TESTS_RUN(tests_psa_crypto_enc_dec_single_key());
    TESTS_RUN(tests_psa_crypto_enc_dec_keypair());
    TESTS_RUN(tests_psa_crypto_enc_dec_protected_key());
    TESTS_END();
    return 0;
}
