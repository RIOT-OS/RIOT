/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_dev_random  /dev/random-like interface
 * @ingroup     sys
 * @brief       Pulls in and unifies an API randomness from various hardware
 *              sources.
 *
 * This API can be used if a board.h defines a `DEV_RANDOM_SRC`
 * macro. As long as there is a way to interpret this randomness source as a
 * random value this API can use it to provide random numbers to higher layer
 * functionality as e.g. seeds for pseudo-random number generators or
 * cryptography keys. @ref drivers_periph_hwrng is used as a default solution.
 *
 * @{
 *
 * @file
 * @brief   dev random definitions.
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */
#ifndef DEV_RANDOM_H_
#define DEV_RANDOM_H_

#include <assert.h>
#include <string.h>

#include "board.h"
#include "dev_random_src.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Source for randomness
 *
 * Should be defined in a board's board.h (but should be guarded by `ifndef`s
 * so that a user may use the randomness source for other applications and
 * redefine it to another source). @ref drivers_periph_hwrng is used as default.
 *
 * See @ref dev_random_src.h for supported values
 */
#ifndef DEV_RANDOM_SRC
#define DEV_RANDOM_SRC  (0)
#error  "No randomness source defined"
#endif

#if DEV_RANDOM_SRC == DEV_RANDOM_SRC_PERIPH_HWRNG
#include "periph/hwrng.h"
#endif

#if DEV_RANDOM_SRC == DEV_RANDOM_SRC_PERIPH_ADC
#include "bitfield.h"
#include "periph/adc.h"
#endif

#if (DEV_RANDOM_SRC == DEV_RANDOM_SRC_AT86RF2XX)
# if defined(MODULE_AT86RF231) || defined(MODULE_AT86RF233)
#include "at86rf2xx.h"
#include "at86rf2xx_internal.h"

extern at86rf2xx_t *dev_random_at86rf2xx;   /**< supposed to be set with device initialization */
# else
#  error  "DEV_RANDOM_SRC set to DEV_RANDOM_SRC_AT86RF2XX, but supporting device not present."
# endif
#endif

/**
 * @brief   Initialize randomness source
 */
static inline void dev_random_init(void)
{
#if DEV_RANDOM_SRC == DEV_RANDOM_SRC_PERIPH_HWRNG
    hwrng_init();
#elif DEV_RANDOM_SRC == DEV_RANDOM_SRC_PERIPH_ADC
    adc_init(DEV_RANDOM_ADC);
#elif   DEV_RANDOM_SRC == DEV_RANDOM_SRC_AT86RF2XX
    assert(dev_random_at86rf2xx);
#else
    return;
#endif
}

/**
 * @brief   Read N bytes of random data from the device
 *
 * @param[in] buf   destination buffer to write the bytes to
 * @param[in] num   number of bytes to get from device
 */
static inline void dev_random_read(void *buf, size_t num)
{
#if DEV_RANDOM_SRC == DEV_RANDOM_SRC_PERIPH_HWRNG
    hwrng_read(buf, (unsigned int)num);
#elif DEV_RANDOM_SRC == DEV_RANDOM_SRC_PERIPH_ADC
    adc_res_t res = ADC_RES_16BIT;
    for (unsigned i = 0; i < num * 8; i++) {
        unsigned *ibuf = buf;
        unsigned ibuf_idx = i / (sizeof(unsigned) * 8);
        unsigned v;
        int r = -1;

        while ((r < 0) && (res >= ADC_RES_6BIT)) {
            r = adc_sample(DEV_RANDOM_ADC, res);
            if (r < 0) {
                res--;
            }
        }
        assert(res >= ADC_RES_6BIT);

        ibuf[ibuf_idx] ^= (v << (i % (sizeof(unsigned) * 8)));
        if (i % (sizeof(unsigned) * 8)) {
            ibuf[ibuf_idx] = (v >> (sizeof(unsigned) * 8) - (i % (sizeof(unsigned) * 8)));
        }
    }
#elif   DEV_RANDOM_SRC == DEV_RANDOM_SRC_AT86RF2XX
    at86rf2xx_get_random(dev_random_at86rf2xx, buf, num);
#else
    memset(buf, 0, num);
#endif
}

#ifdef __cplusplus
}
#endif

#endif /* DEV_RANDOM_H_ */
/** @} */
