/*
 * SPDX-FileCopyrightText: 2023 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
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
