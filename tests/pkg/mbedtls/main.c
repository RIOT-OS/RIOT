/*
 * SPDX-FileCopyrightText: 2020 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @ingroup     tests
 * @{
 *
 * @file
 * @brief       Test the correct loading and linking of the mbedtls package and
                execute built-in selftests.
 *
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 *
 * @}
 */

#include <stdio.h>

#include "mbedtls/sha256.h"
#include "mbedtls/entropy.h"

#include "entropy_mbedtls_riot.h"

int main(void)
{
    puts("mbedtls test\n");

    /* Execute built-in tests SHA-2xx tests */
    mbedtls_sha256_self_test(1);

    bool adc_noise_single_entropy = false;
    /* NOTE, the following express needs adjustment when further entropy
       sources added by RIOT */
#if IS_USED(MODULE_MBEDTLS_ENTROPY_SOURCE_ADC) && !IS_USED(MODULE_MBEDTLS_ENTROPY_SOURCE_HWRNG)
    /* If the adc noise entropy source is compiled and is the only source
       utilized with the mbedtls entropy module, indicate it */
    adc_noise_single_entropy = true;
#endif
    /* Used by the testrunner to ignore potential test failures it the case of
       a single adc noise entropy source */
    printf("adc_noise_single_entropy: %d\n", adc_noise_single_entropy);

    /* Initializes available entropy sources */
    entropy_mbedtls_riot_init();

    /* Execute mbedtls built-in entropy tests*/
    mbedtls_entropy_self_test(1);

    /* Execute convenience entropy functions */
    unsigned char out[4];
    /* This function requires entropy_mbedtls_riot_init() beforehand */
    entropy_mbedtls_riot_retrieve(out, sizeof(out));
    /* Clears the internal context again */
    entropy_mbedtls_riot_uninit();

    /* This function does not require initialization */
    entropy_mbedtls_riot_get(out, sizeof(out));

    return 0;
}
