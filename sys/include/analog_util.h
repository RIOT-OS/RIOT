/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup    sys_analog_util Analog data conversion utilities
 * @ingroup     sys
 * @brief       Utility functions for converting analog data samples
 *
 * @{
 * @file
 * @brief       Analog utility function interfaces
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#include <stdint.h>

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
 * @param[in] sample        sampled ADC value
 * @param[in] res           ADC resolution
 * @param[in] min           the lower bound of the target interval
 * @param[in] max           the upper bound of the target interval
 *
 * @return                  the mapped value
 */
int32_t adc_util_map(int sample, adc_res_t res, int32_t min, int32_t max);

/**
 * @brief   Map a sampled ADC value to a given range (using floating point
 *          arithmetic)
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

/**
 * @brief   Map a value out of the given range to a 16-bit unsigned int
 *
 * The min value is assumed to be smaller than max value and value is assumed
 * to be between min and max.
 *
 * @param[in] value        value to map to a DAC set value
 * @param[in] min          the lower bound of the source interval
 * @param[in] max          the upper bound of the source interval
 *
 * @return                 the mapped value
 */
uint16_t dac_util_map(int value, int min, int max);

/**
 * @brief Helper function to map a given float value range to a valid DAC value.
 *
 * @see dac_util_map
 *
 * @param[in] value        value to map to a DAC set value
 * @param[in] min          the lower bound of the source interval
 * @param[in] max          the upper bound of the source interval
 *
 * @return                 the mapped value
 */
uint16_t dac_util_mapf(float value, float min, float max);

/**
 * @brief   Generate a sine wave for the DAC
 *
 * This function fills the provided buffer with samples of a sine wave
 * based on the specified center voltage, amplitude, and frequency.
 *
 * @param[in] center_mv     Center voltage of the sine wave in millivolts
 * @param[in] amplitude_mv  Amplitude of the sine wave in millivolts (max - center)
 * @param[in] freq_hz       Frequency of the sine wave in hertz
 * @param[in] dac_ref_mv    Reference voltage of the DAC in millivolts
 * @param[out] buf          Buffer to store the generated samples
 * @param[in,out] buf_max   Pointer to maximum buffer capacity on entry.
 *                          - On input: Maximum number of samples the buffer can hold.
 *                          - On output: Actual number of samples generated.
 *
 * @retval 0                Success
 * @retval -EINVAL          Invalid parameters, e.g. @p center_mv < @p amplitude_mv
 * @retval -ENOMEM          Insufficient buffer size for one period of the requested frequency
 */
int dac_util_sine(uint16_t center_mv, uint16_t amplitude_mv, uint32_t freq_hz,
                  uint16_t dac_ref_mv, uint16_t *buf, uint32_t *buf_max);

#ifdef __cplusplus
}
#endif

/** @} */
