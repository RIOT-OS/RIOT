/*
 * Copyright (C) 2022 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     examples
 * @{
 *
 * @brief       Example application for PSA Crypto
 *
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 * @}
 */

#include <stdio.h>
#include "psa/crypto.h"
#include "ztimer.h"

#if IS_USED(MODULE_PSA_CIPHER)
extern psa_status_t example_cipher_aes_128(void);
#endif
#if IS_USED(MODULE_PSA_MAC)
extern psa_status_t example_hmac_sha256(void);
#endif
#if IS_USED(MODULE_PSA_ASYMMETRIC_ECC_P256R1)
extern psa_status_t example_ecdsa_p256(void);
#endif
#if IS_USED(MODULE_PSA_ASYMMETRIC_ECC_ED25519)
#ifndef SECURE_ELEMENT
extern psa_status_t example_eddsa(void);
#endif
#endif

#ifdef MULTIPLE_SE
#if IS_USED(MODULE_PSA_CIPHER)
extern psa_status_t example_cipher_aes_128_sec_se(void);
#endif /* MODULE_PSA_CIPHER */
#if IS_USED(MODULE_PSA_MAC)
extern psa_status_t example_hmac_sha256_sec_se(void);
#endif /* MODULE_PSA_MAC */
#if IS_USED(MODULE_PSA_ASYMMETRIC_ECC_P256R1)
extern psa_status_t example_ecdsa_p256_sec_se(void);
#endif /* MODULE_PSA_ASYMMETRIC_ECC_P256R1 */
#endif /* MULTIPLE_SE */

int main(void)
{
    bool failed = false;
    psa_status_t status;

    psa_crypto_init();

    ztimer_acquire(ZTIMER_USEC);
    ztimer_now_t start = ztimer_now(ZTIMER_USEC);

    /* Needed in case only hashes are tested */
    (void)status;
    (void)start;

#if IS_USED(MODULE_PSA_MAC)
    status = example_hmac_sha256();
    printf("HMAC SHA256 took %d us\n", (int)(ztimer_now(ZTIMER_USEC) - start));
    if (status != PSA_SUCCESS) {
        failed = true;
        printf("HMAC SHA256 failed: %s\n", psa_status_to_humanly_readable(status));
    }
#endif

#if IS_USED(MODULE_PSA_CIPHER)
    start = ztimer_now(ZTIMER_USEC);
    status = example_cipher_aes_128();
    printf("Cipher AES 128 took %d us\n", (int)(ztimer_now(ZTIMER_USEC) - start));
    if (status != PSA_SUCCESS) {
        failed = true;
        printf("Cipher AES 128 failed: %s\n", psa_status_to_humanly_readable(status));
    }
#endif

#if IS_USED(MODULE_PSA_ASYMMETRIC_ECC_P256R1)
    start = ztimer_now(ZTIMER_USEC);
    status = example_ecdsa_p256();
    printf("ECDSA took %d us\n", (int)(ztimer_now(ZTIMER_USEC) - start));
    if (status != PSA_SUCCESS) {
        failed = true;
        printf("ECDSA failed: %s\n", psa_status_to_humanly_readable(status));
    }
#endif

#if IS_USED(MODULE_PSA_ASYMMETRIC_ECC_ED25519)
    start = ztimer_now(ZTIMER_USEC);
    status = example_eddsa();
    printf("EdDSA took %d us\n", (int)(ztimer_now(ZTIMER_USEC) - start));
    if (status != PSA_SUCCESS) {
        failed = true;
        printf("EdDSA failed: %s\n", psa_status_to_humanly_readable(status));
    }
#endif

#ifdef MULTIPLE_SE
#if IS_USED(MODULE_PSA_MAC)
    puts("Running Examples with secondary SE:");
    status = example_hmac_sha256_sec_se();
    printf("HMAC SHA256 took %d us\n", (int)(ztimer_now(ZTIMER_USEC) - start));
    if (status != PSA_SUCCESS) {
        failed = true;
        printf("HMAC SHA256 failed: %s\n", psa_status_to_humanly_readable(status));
    }
#endif /* MODULE_PSA_MAC */

#if IS_USED(MODULE_PSA_CIPHER)
    start = ztimer_now(ZTIMER_USEC);
    status = example_cipher_aes_128_sec_se();
    printf("Cipher AES 128 took %d us\n", (int)(ztimer_now(ZTIMER_USEC) - start));
    if (status != PSA_SUCCESS) {
        failed = true;
        printf("Cipher AES 128 failed: %s\n", psa_status_to_humanly_readable(status));
    }
#endif /* MODULE_PSA_CIPHER */

#if IS_USED(MODULE_PSA_ASYMMETRIC_ECC_P256R1)
    start = ztimer_now(ZTIMER_USEC);
    status = example_ecdsa_p256_sec_se();
    printf("ECDSA took %d us\n", (int)(ztimer_now(ZTIMER_USEC) - start));
    if (status != PSA_SUCCESS) {
        failed = true;
        printf("ECDSA failed: %s\n", psa_status_to_humanly_readable(status));
    }
#endif /* MODULE_PSA_ASYMMETRIC_ECC_P256R1 */
#endif /* MULTIPLE_SE */

    ztimer_release(ZTIMER_USEC);

    if (failed) {
        puts("Tests failed...");
    }
    else {
        puts("All Done");
    }
    return 0;
}
