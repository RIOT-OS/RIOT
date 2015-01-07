/*
 * Copyright (C) 2014 Simon Brummer
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    driver_periph_dac DAC
 * @ingroup     driver_periph
 * @brief       Low-level DAC peripheral driver
 *
 * @{
 * @file
 * @brief       Low-level DAC peripheral driver interface definitions
 *
 * @author      Simon Brummer <simon.brummer@haw-hamburg.de>
 */

#ifndef __DAC_H
#define __DAC_H

#include <stdint.h>
#include "periph_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/* guard file in case no DAC device is defined */
#if DAC_NUMOF

/**
 * @brief Definition avialable DAC devices
 *
 * Each DAC device is based on a hardware DAC which can have one or more
 * independet channels.
 */
typedef enum {
#if DAC_0_EN
    DAC_0 = 0,             /**< DAC device 0 */
#endif
#if DAC_1_EN
    DAC_1 = 1,             /**< DAC device 1 */
#endif
#if DAC_2_EN
    DAC_2 = 2,             /**< DAC device 2 */
#endif
#if DAC_3_EN
    DAC_3 = 3,             /**< DAC device 3 */
#endif
} dac_t;

/**
 * @brief Possilbe DAC precision settings
 */
typedef enum {
    DAC_RES_6BIT = 0,      /**< DAC precision: 6 bit */
    DAC_RES_8BIT,          /**< DAC precision: 8 bit */
    DAC_RES_10BIT,         /**< DAC precision: 10 bit */
    DAC_RES_12BIT,         /**< DAC precision: 12 bit */
    DAC_RES_14BIT,         /**< DAC precision: 14 bit */
    DAC_RES_16BIT,         /**< DAC precision: 16 bit */
} dac_precision_t;

/**
 * @brief Initialization of a given DAC device
 *
 * The DAC will be initialized with all possilble channels active. 
 * On each channel will be initialized with a Zero on it.
 *
 * @param[in] dev          the device to initialize
 * @param[in] precision    the precision to use for conversion
 *
 * @return                 0 on success
 * @return                 -1 on unknown DAC Device
 * @return                 -2 on precision not available
 */
int8_t dac_init(dac_t dev, dac_precision_t precision);

/**
 * @brief Write a value onto DAC Device on a given Channel.
 *
 * @param[in] dev          the DAC device to use
 * @param[in] channel      the channel used for Digital/Analoge conversion 
 * @param[in] value        the value to write onto DAC. 
 * 
 * @return                 0 on success
 * @return                 -1 on unknown DAC Device
 * @return                 -2 on invalid channel 
 * @return                 -3 if value is out of range. 
 */
int8_t dac_write(dac_t dev, uint8_t channel, uint16_t value);

/**
 * @brief Enable power for the given DAC Device
 * 
 * @param[in] dev          the DAC device to power up.
 *
 * @return                 0 on success
 * @return                 -1 on unknown DAC Device
 */
int8_t dac_poweron(dac_t dev);

/**
 * @brief Disable power for the given DAC Device
 *
 * @param[in] dev          the DAC device to power down
 * 
 * @return                 0 on success
 * @return                 -1 on unknown DAC Device
 */
int8_t dac_poweroff(dac_t dev);

/**
 * @brief Helper function to map a given integer range to a valid DAC value.
 * 
 * This function is useful for converting ranges of values to a valid DAC output value.
 *
 * The min value is assumed to be smaller than max value and value is assumed 
 * to be between min and max.
 *
 * @param[in] dev          the DAC Device to read precision value from
 * @param[in] value        the value to map onto min and max
 * @param[in] min          the lower bound of the target interval
 * @param[in] max          the upper bound of the target interval
 *
 * @return                 the mapped value, in valid DAC range
 */
uint16_t dac_map(dac_t dev, int value, int min, int max);

/**
 * @brief Helper function to map a given float value range to a valid DAC value.
 * 
 * @see dac_map
 *
 * @param[in] dev          the DAC Device to read precision value from
 * @param[in] value        the value to map onto min and max
 * @param[in] min          the lower bound of the target interval
 * @param[in] max          the upper bound of the target interval
 * 
 * @return                 the mapped value, in valid DAC range
 */
uint16_t dac_mapf(dac_t dev, float value, float min, float max);

#endif/* DAC_NUMOF */ 
#ifdef __cplusplus
}
#endif
#endif /* __DAC_H */
/** @} */
