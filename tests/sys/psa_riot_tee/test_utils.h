/*
 * Copyright (C) 2024 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup
 * @defgroup       <name> <description>
 * @{
 *
 * @file        test_utils.h
 * @brief
 *
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 */

#ifndef TEST_UTILS_H
#define TEST_UTILS_H

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

Test* tests_psa_import_key(void);
Test* tests_psa_generate_sealed_key(void);
Test* tests_psa_aes_encrypt_with_hw_key(void);
Test* tests_psa_hash_gen_with_hw(void);

#ifdef __cplusplus
}
#endif

#endif /* TEST_UTILS_H */
/** @} */
