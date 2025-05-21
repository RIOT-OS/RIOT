/*
 * Copyright (C) 2020 Bas Stottelaar <basstottelaar@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup    cpu_efm32_drivers_coretemp EFM32 internal temperature sensor
 * @ingroup     cpu_efm32_drivers
 * @ingroup     drivers_saul
 * @brief       Driver for the EFM32 internal temperature sensor
 *
 * All EFM32 chips have an internal ADC input channel that reads the internal
 * temperature sensor. This EFM32-specific driver provides an interface for
 * reading this value, compensated using factory-calibrated values.
 *
 * The board must define `CORETEMP_ADC` to point to the ADC line that connects
 * to the right ADC input channel.
 *
 * This driver provides @ref drivers_saul capabilities.
 *
 * @{
 *
 * @file
 * @brief       Interface definition of the EFM32 internal temperature sensor
 *              driver.
 *
 * @author      Bas Stottelaar <basstottelaar@gmail.com>
 */


#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Initialize the sensor.
 *
 * @return                  0 on successful initialization
 * @return                  -EIO on ADC initialization error
 *
 * This driver assumes that the `CORETEMP_ADC` is defined and points to the ADC
 * input channel that is connected to the internal temperature sensor.
 */
int coretemp_init(void);

/**
 * @brief   Read the current temperature from the sensor.
 *
 * @return                  current temperature in centi-degrees Celsius
 *                          (times 100)
 *
 * Temperature readings are compensated using the factory-calibration values.
 */
int16_t coretemp_read(void);

#ifdef __cplusplus
}
#endif

/** @} */
