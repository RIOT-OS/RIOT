/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the LGPLv2 License.
 * See the file LICENSE in the top level directory for more details.
 */

/**
 * @ingroup     driver_periph
 * @brief       Low-level ADC peripheral driver
 * @{
 *
 * @file        adc.h
 * @brief       Low-level ADC peripheral driver interface definitions
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef __ADC_H
#define __ADC_H

#include "periph_conf.h"


/**
 * @brief Definition available ADC devices
 *
 * Each ADC device is based on a hardware ADC which can have one or more
 * multiplexed channels.
 */
typedef enum {
#if ADC_0_EN
    ADC_0,              /*< ADC device 0 */
#endif
#if ADC_1_EN
    ADC_1,              /*< ADC device 1 */
#endif
#if ADC_2_EN
    ADC_2,               /*< ADC device 2 */
#endif
#if ADC_3_EN
    ADC_3,               /*< ADC device 3 */
#endif
    ADC_UNDEFINED
} adc_t;

/**
 * @brief Initialization of a given ADC device
 *
 * The ADC will be initialized in synchronous, blocking mode, so no callbacks for finished
 * conversions are required as conversions are presumed to be very fast (somewhere in the
 * range of some us).
 *
 * @param[in] dev           the device to initialize
 * @param[in] precision     the conversion precision in bits, will LSBs will be stuffed if
 *                          precision > max precision of hardware
 *
 * @return                  1 on success, -1 on error
 */
int adc_init(adc_t dev, int precision);

/**
 * @brief Start a new conversion by using the given channel.
 *
 * If a conversion on any channel on the given ADC core is in progress, it is aborted.
 *
 * @param[in] dev           the ADC device to use for the conversion
 * @param[in] channel       the channel to convert from
 *
 * @return                  the converted value with the defined precision, -1 on error
 */
int adc_sample(adc_t dev, int channel);

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

#endif /* __ADC_H */
/** @} */
