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
 * @brief       Tests the PSA HMAC SHA256 configurations
 *
 * @author      Mikolai Gütschow <mikolai.guetschow@tu-dresden.de>
 * @author      Lena Boeckmann <lena.boeckmann@haw-hamburg.de>
 *
 * @}
 */

#include <stdio.h>
#include "psa/crypto.h"
#include "ztimer.h"
#include "tests_gen_hmac.h"

int main(void)
{
    ztimer_now_t start;

    psa_crypto_init();

    ztimer_acquire(ZTIMER_USEC);
    start = ztimer_now(ZTIMER_USEC);

    TESTS_START();
    TESTS_RUN(tests_mac_hmac_sha512());
    TESTS_RUN(tests_mac_hmac_sha384());
    TESTS_RUN(tests_mac_hmac_sha256());
    TESTS_END();

    printf("HMAC tests took %d us\n", (int)(ztimer_now(ZTIMER_USEC) - start));
    ztimer_release(ZTIMER_USEC);

    return 0;
}
