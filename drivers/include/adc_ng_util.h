/*
 * Copyright (C) 2020 Otto-von-Guericke-Universität Magdeburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    drivers_adc_ng_util Utility functions for ADC NG
 * @ingroup     drivers_adc_ng
 *
 * This module contains contains utility functions for the ADC NG API.
 *
 * @{
 *
 * @file
 * @brief       Interface definition of the ADC utility API
 *
 * @author      Marian Buschsieweke <marian.buschsieweke@ovgu.de>
 */

#ifndef ADC_NG_UTIL_H
#define ADC_NG_UTIL_H

#include <assert.h>
#include <errno.h>
#include <stdint.h>

#include "adc_ng.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Measure the MCU's supply voltage using the given ADC
 *
 * @param[in]       adc     ADC to use
 * @param[out]      dest_mv Measured supply voltage in mV will be stored here
 *
 * @retval  0               Success
 * @retval  -ENOTSUP        Measuring the supply voltage is not supported using
 *                          the ADC @p adc
 * @retval  <0              Error
 *
 * @pre     The ADC is currently offline, it will internally be initialized
 */
int adc_ng_vcc(uint8_t adc, int16_t *dest_mv);

/**
 * @brief   Measure the temperature using a thermistor
 *
 * @param[in]       adc     ADC device to use
 * @param[in]       chan    Channel the NTC is connected to
 * @param[in]       ntc     Parameters of the NTC used
 * @param[out]      temp    The temperature in 0.1°C will be stored here
 *
 * @retval  0               Success
 * @retval  <0              Error
 *
 * @pre     The ADC identified by @p adc is currently offline
 */
int adc_ng_ntc(uint8_t adc, uint8_t chan, const adc_ng_ntc_t *ntc, int16_t *temp);

/**
 * @brief   Measure the temperature using a thermistor internally connected to
 *          the ADC
 *
 * @param[in]       adc     ADC device to use
 * @param[out]      temp    The temperature in 0.1°C will be stored here
 *
 * @retval  0               Success
 * @retval  -ENOTSUP        No internal NTC connected to the given ADC
 * @retval  <0              Other device specific error
 *
 * @pre     The ADC identified by @p adc is currently offline
 */
static inline int adc_ng_internal_ntc(uint8_t adc, int16_t *temp)
{
    assert(adc < XFA_LEN(adc_ng_backend_t, adc_ng_backends));
    const adc_ng_backend_t be = adc_ng_backends[adc];
    if (!be.driver->ntc) {
        return -ENOTSUP;
    }

    return adc_ng_ntc(adc, ADC_NG_CHAN_NTC, be.driver->ntc, temp);
}

/**
 * @brief   Use the ADC to obtain entropy
 *
 * @param[in]       adc     ADC device to use
 * @param[out]      dest    Buffer to store the collected entropy into
 * @param[in]       size    Size of @p dest
 *
 * @retval  0               Success
 * @retval  -ENOTSUP        The given ADC does not support collecting entropy
 * @retval  <0              Other device specific error
 *
 * @pre     The ADC identified by @p adc is currently offline
 *
 * @warning The quality of the entropy generated from an ADC can depend on
 *          many factors and even under ideal circumstances is usually not
 *          well suited for direct use in cryptographic contexts. However, as
 *          an additionally source of weak entropy used by an entropy extractor,
 *          this should work well.
 */
int adc_ng_entropy(uint8_t adc, void *dest, size_t size);

#ifdef __cplusplus
}
#endif

#endif /* ADC_NG_UTIL_H */
/** @} */
