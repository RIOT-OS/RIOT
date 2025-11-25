/*
 * SPDX-FileCopyrightText: 2022 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @brief       Tests the PSA cipher configurations
 *
 * @author      Mikolai Gütschow <mikolai.guetschow@tu-dresden.de>
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 * @}
 */

#include <stdio.h>
#include "psa/crypto.h"
#include "ztimer.h"

extern psa_status_t example_cipher_aes_128(void);
extern psa_status_t example_cipher_chacha20_oneshot(void);
extern psa_status_t example_cipher_chacha20_multipart(void);

int main(void)
{
    bool failed = false;
    psa_status_t status;

    ztimer_acquire(ZTIMER_USEC);
    ztimer_now_t start = ztimer_now(ZTIMER_USEC);

    start = ztimer_now(ZTIMER_USEC);
    status = example_cipher_aes_128();
    printf("Cipher AES 128 took %d us\n", (int)(ztimer_now(ZTIMER_USEC) - start));
    if (status != PSA_SUCCESS) {
        failed = true;
        printf("Cipher AES 128 failed: %s\n", psa_status_to_humanly_readable(status));
    }

    start = ztimer_now(ZTIMER_USEC);
    status = example_cipher_chacha20_oneshot();
    printf("Cipher CHACHA20 oneshot took %d us\n", (int)(ztimer_now(ZTIMER_USEC) - start));
    if (status != PSA_SUCCESS) {
        failed = true;
        printf("Cipher CHACHA20 failed: %s\n", psa_status_to_humanly_readable(status));
    }

    start = ztimer_now(ZTIMER_USEC);
    status = example_cipher_chacha20_multipart();
    printf("Cipher CHACHA20 multipart took %d us\n", (int)(ztimer_now(ZTIMER_USEC) - start));
    if (status != PSA_SUCCESS) {
        failed = true;
        printf("Cipher CHACHA20 failed: %s\n", psa_status_to_humanly_readable(status));
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
