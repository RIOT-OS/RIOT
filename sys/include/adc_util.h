/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    sys_adc_util ADC utilities
 * @ingroup     sys
 * @brief       Utility functions for handling ADC samples
 *
 * @{
 * @file
 * @brief       ADC utility function interfaces
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef ADC_UTIL_H
#define ADC_UTIL_H

#include "periph/adc.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Map a sampled ADC value to a given range
 *
 * This function is useful for converting sampled ADC values into their physical
 * representation.
 *
 * The min value is asserted to be smaller than the max value.
 *
 * @param[in] sample        sampled ADC value
 * @param[in] res           ADC resolution
 * @param[in] min           the lower bound of the target interval
 * @param[in] max           the upper bound of the target interval
 *
 * @return                  the mapped value
 */
int adc_util_map(int sample, adc_res_t res, int min, int max);

/**
 * @brief   Map a sampled ADC value to a given range (using floating point
 *          arithmetics)
 *
 * @see adc_util_map
 *
 * @param[in] sample        sampled ADC value
 * @param[in] res           ADC resolution
 * @param[in] min           the lower bound of the target interval
 * @param[in] max           the upper bound of the target interval
 *
 * @return                  the mapped value
 */
float adc_util_mapf(int sample, adc_res_t res, float min, float max);

#ifdef __cplusplus
}
#endif

#endif /* ADC_UTIL_H */
/** @} */
