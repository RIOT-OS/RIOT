/*
 * Copyright (C) 2020 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     pkg_mbedtls_entropy
 *
 * @{
 * @file
 * @brief
 *
 * @author      Peter Kietzmann <peter.kietzmann@haw-hamburg.de>
 *
 * @}
 */
#include <assert.h>

#include "mbedtls/entropy.h"
#include "mbedtls/entropy_poll.h"

#include "entropy_sources_mbedtls_riot.h"

#include "kernel_defines.h"
#if IS_USED(MODULE_MBEDTLS_ENTROPY_SOURCE_HWRNG)
#include "periph/hwrng.h"
#endif

#if IS_USED(MODULE_MBEDTLS_ENTROPY_SOURCE_ADC)
#include "entropy_source/adc_noise.h"
#endif

/**
 * @brief   Number of available entropy sources.
 */
#define NUM_ENTROPY_FUNCS ARRAY_SIZE(entropy_funcs)

/**
 * @brief   Configuration of available entropy sources.
 *
 * @note    First source must be the best source available. It is
 *          expected to be strong.
 * @note    An entropy source is configured if the platform provides
            the underlying feature and the PSEUDOMODULE (which is
            included by default) is not manually disabled.
 * @{
 */
const entropy_source_mbedtls_riot_t entropy_funcs[] = {
#if IS_USED(MODULE_MBEDTLS_ENTROPY_SOURCE_HWRNG)
{ .func = riot_hwrng_poll, .strong = MBEDTLS_ENTROPY_SOURCE_STRONG },
#endif
#if IS_USED(MODULE_MBEDTLS_ENTROPY_SOURCE_ADC)
{ .func = riot_adc_poll, .strong = MBEDTLS_ENTROPY_SOURCE_WEAK },
#endif
    /* Additional sources need to be added here */
};
/** @} */

/* Connect first entropy source because this function
 * is used for internal tests
 */
int mbedtls_hardware_poll(void *data, unsigned char *output, size_t len,
                            size_t *olen)
{
    assert(output != NULL && olen!= NULL);

    return entropy_funcs[0].func(data, output, len, olen);
}

/* Add more entropy sources. Start from index=1 because index=0 is used in
 * mbedtls_hardware_poll already
 */
int riot_add_entropy_src_avail(mbedtls_entropy_context *ctx)
{
    assert(ctx);
    int ret = 0;

#if IS_USED(MODULE_MBEDTLS_ENTROPY_SOURCE_ADC)
    entropy_source_adc_init();
#endif

    for (unsigned i = 1; i < NUM_ENTROPY_FUNCS; i++) {
        ret = mbedtls_entropy_add_source(ctx, entropy_funcs[i].func, NULL,
                                         1, entropy_funcs[i].strong);
        if (ret != 0) {
            return ret;
        }
    }

    return ret;
}

#if IS_USED(MODULE_MBEDTLS_ENTROPY_SOURCE_HWRNG)
int riot_hwrng_poll(void *data, unsigned char *output, size_t len,
                        size_t *olen)
{

    assert(output != NULL && olen!= NULL);

    (void)data;

    hwrng_read(output, len);
    *olen = len;

    return 0;
}
#endif

#if IS_USED(MODULE_MBEDTLS_ENTROPY_SOURCE_ADC)
int riot_adc_poll(void *data, unsigned char *output, size_t len,
                    size_t *olen)
{
    assert(output != NULL && olen!= NULL);

    (void)data;

    if (entropy_source_adc_get(output, len) < 0) {
        return MBEDTLS_ERR_ENTROPY_SOURCE_FAILED;
    }
    *olen=len;

    return 0;
}
#endif
