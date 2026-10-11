/*
 * SPDX-FileCopyrightText: 2025 TU Dresden
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @brief       Tests the PSA aead configurations
 *
 * @author      Lukas Luger <lukas.luger@mailbox.tu-dresden.de>
 *
 * @}
 */

#include <stdio.h>
#include "psa/crypto.h"
#include "ztimer.h"

extern psa_status_t example_aead_aes_128_ccm(void);
extern psa_status_t example_aead_aes_gcm_128_oneshot(void);
extern psa_status_t example_aead_aes_gcm_192_oneshot(void);
extern psa_status_t example_aead_aes_gcm_256_oneshot(void);

int main(void)
{
    bool failed = false;
    psa_status_t status;

    psa_crypto_init();

    ztimer_acquire(ZTIMER_USEC);
    ztimer_now_t start = ztimer_now(ZTIMER_USEC);

    start = ztimer_now(ZTIMER_USEC);
    status = example_aead_aes_128_ccm();
    printf("Authenticated encryption with associated data AES 128 CCM took %d us\n",
           (int)(ztimer_now(ZTIMER_USEC) - start));
    if (status != PSA_SUCCESS) {
        failed = true;
        printf("Authenticated encryption with associated data AES 128 CCM failed: %s\n",
               psa_status_to_humanly_readable(status));
    }

    start = ztimer_now(ZTIMER_USEC);
    status = example_aead_aes_gcm_128_oneshot();
    printf("Authenticated encryption with associated data AES 128 GCM took %d us\n",
           (int)(ztimer_now(ZTIMER_USEC) - start));
    if (status != PSA_SUCCESS) {
        failed = true;
        printf("Authenticated encryption with associated data AES 128 GCM failed: %s\n",
               psa_status_to_humanly_readable(status));
    }

    start = ztimer_now(ZTIMER_USEC);
    status = example_aead_aes_gcm_192_oneshot();
    printf("Authenticated encryption with associated data AES 192 GCM took %d us\n",
           (int)(ztimer_now(ZTIMER_USEC) - start));
    if (status != PSA_SUCCESS) {
        failed = true;
        printf("Authenticated encryption with associated data AES 192 GCM failed: %s\n",
               psa_status_to_humanly_readable(status));
    }

    start = ztimer_now(ZTIMER_USEC);
    status = example_aead_aes_gcm_256_oneshot();
    printf("Authenticated encryption with associated data AES 256 GCM took %d us\n",
           (int)(ztimer_now(ZTIMER_USEC) - start));
    if (status != PSA_SUCCESS) {
        failed = true;
        printf("Authenticated encryption with associated data AES 256 GCM failed: %s\n",
               psa_status_to_humanly_readable(status));
    }

    ztimer_release(ZTIMER_USEC);

    if (failed) {
        puts("Tests failed...");
    }
    else {
        puts("All Done");
    }
    return 0;
}
