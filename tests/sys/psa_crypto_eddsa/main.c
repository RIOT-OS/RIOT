/*
 * Copyright (C) 2022 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     tests
 * @{
 *
 * @brief       Tests the PSA EDDSA configurations
 *
 * @author      Mikolai Gütschow <mikolai.guetschow@tu-dresden.de>
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 * @}
 */

#include <stdio.h>
#include "psa/crypto.h"
#include "ztimer.h"

extern psa_status_t example_eddsa(void);

int main(void)
{
    bool failed = false;
    psa_status_t status;

    psa_crypto_init();

    ztimer_acquire(ZTIMER_USEC);
    ztimer_now_t start = ztimer_now(ZTIMER_USEC);

    start = ztimer_now(ZTIMER_USEC);
    status = example_eddsa();
    printf("EdDSA took %d us\n", (int)(ztimer_now(ZTIMER_USEC) - start));
    if (status != PSA_SUCCESS) {
        failed = true;
        printf("EdDSA failed: %s\n", psa_status_to_humanly_readable(status));
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
