/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    driver_periph_adc ADC
 * @ingroup     driver_periph
 * @brief       Low-level ADC peripheral driver
 * @{
 *
 * @file
 * @brief       Low-level ADC peripheral driver interface definitions
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef __ADC_H
#define __ADC_H

#include "periph_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/* guard file in case no ADC device is defined */
#if ADC_NUMOF

/**
 * @brief Definition available ADC devices
 *
 * Each ADC device is based on a hardware ADC which can have one or more
 * multiplexed channels.
 */
typedef enum {
#if ADC_0_EN
    ADC_0 = 0,              /**< ADC device 0 */
#endif
#if ADC_1_EN
    ADC_1,                  /**< ADC device 1 */
#endif
#if ADC_2_EN
    ADC_2,                  /**< ADC device 2 */
#endif
#if ADC_3_EN
    ADC_3,                  /**< ADC device 3 */
#endif
} adc_t;

/**
 * @brief Possible ADC precision settings
 */
typedef enum {
    ADC_RES_6BIT = 0,       /**< ADC precision: 6 bit */
    ADC_RES_8BIT,           /**< ADC precision: 8 bit */
    ADC_RES_10BIT,          /**< ADC precision: 10 bit */
    ADC_RES_12BIT,          /**< ADC precision: 12 bit */
    ADC_RES_14BIT,          /**< ADC precision: 14 bit */
    ADC_RES_16BIT,          /**< ADC precision: 16 bit */
} adc_precision_t;

/**
 * @brief Initialization of a given ADC device
 *
 * The ADC will be initialized in synchronous, blocking mode, so no callbacks for finished
 * conversions are required as conversions are presumed to be very fast (somewhere in the
 * range of some us).
 *
 * @param[in] dev           the device to initialize
 * @param[in] precision     the precision to use for conversion
 *
 * @return                  0 on success
 * @return                  -1 on precision not available
 */
int adc_init(adc_t dev, adc_precision_t precision);

/**
 * @brief Start a new conversion by using the given channel.
 *
 * If a conversion on any channel on the given ADC core is in progress, it is aborted.
 *
 * @param[in] dev           the ADC device to use for the conversion
 * @param[in] channel       the channel to convert from
 *
 * @return                  the converted value
 * @return                  -1 on invalid channel
 */
int adc_sample(adc_t dev, int channel);

/**
 * @brief Enable the power for the given ADC device
 *
 * @param[in] dev           the ADC device to power up
 */
void adc_poweron(adc_t dev);

/**
 * @brief Disable the power for the given ADC device
 *
 * @param[in] dev           the ADC device to power down
 */
void adc_poweroff(adc_t dev);

/**
 * @brief Helper function to map a converted value to the given integer range.
 *
 * This function is useful for converting sampled ADC values into their physical representation.
 *
 * The min value is asserted to be smaller than the max value.
 *
 * @param[in] dev           the ADC device to read the precision value from (as input interval)
 * @param[in] value         the value to map
 * @param[in] min           the lower bound of the target interval
 * @param[in] max           the upper bound of the target interval
 *
 * @return                  the mapped value
 */
int adc_map(adc_t dev, int value, int min, int max);

/**
 * @brief Helper function to map a converted value to the given float range
 *
 * @see adc_map
 *
 * @param[in] dev           the ADC device to read the precision value from (as input interval)
 * @param[in] value         the value to map
 * @param[in] min           the lower bound of the target interval
 * @param[in] max           the upper bound of the target interval
 *
 * @return                  the mapped value
 */
float adc_mapf(adc_t dev, int value, float min, float max);

#endif /* ADC_NUMOF */

#ifdef __cplusplus
}
#endif

#endif /* __ADC_H */
/** @} */
