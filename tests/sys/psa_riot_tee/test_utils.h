/*
 * SPDX-FileCopyrightText: 2024 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @file
 * @brief       Shared declarations for the PSA RIOT TEE tests
 *
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 */

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include "embUnit.h"
#include "psa/crypto.h"

#define TEST_ASSERT_PSA_SUCCESS(func_)          TEST_ASSERT_MESSAGE(func_ == PSA_SUCCESS, #func_ " failed");

#define ECDSA_MESSAGE_SIZE  (127)

#define ECC_KEY_SIZE    (256)
#define ECC_KEY_TYPE    (PSA_KEY_TYPE_ECC_KEY_PAIR(PSA_ECC_FAMILY_SECP_R1))
#define ECC_ALG_HASH    (PSA_ALG_SHA_256)
#define ECC_ALG         (PSA_ALG_ECDSA(ECC_ALG_HASH))

Test* tests_psa_import_and_seal_key(void);
Test* tests_psa_generate_sealed_key(void);
Test* tests_psa_aes_128_encrypt_decrypt(void);
Test* tests_psa_hash_sha256_gen(void);

#ifdef __cplusplus
}
#endif
