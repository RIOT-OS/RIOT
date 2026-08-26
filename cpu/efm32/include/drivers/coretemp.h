/*
 * SPDX-FileCopyrightText: 2020 Bas Stottelaar <basstottelaar@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    cpu_efm32_drivers_coretemp EFM32 internal temperature sensor
 * @ingroup     cpu_efm32_drivers
 * @ingroup     drivers_saul
 * @brief       Driver for the EFM32 internal temperature sensor
 *
 * All EFM32 chips have an internal temperature sensor. This EFM32-specific
 * driver provides an interface for reading this value, compensated using
 * factory-calibrated values.
 *
 * On Series 0 and Series 1 devices, the sensor is read through an ADC input
 * channel. The board must define `CORETEMP_ADC` to point to the ADC line
 * that connects to the right ADC input channel.
 *
 * On Series 2 devices, the sensor is read directly through the EMU
 * peripheral.
 *
 * This driver provides @ref drivers_saul capabilities.
 *
 * @note    The sensor measures the die temperature, which is higher than the
 *          ambient temperature when the device is awake and dissipates power.
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
 * On Series 0 and Series 1 devices, this driver assumes that `CORETEMP_ADC`
 * is defined and points to the ADC input channel that is connected to the
 * internal temperature sensor. Series 2 devices require no configuration.
 *
 * @retval      0       successful initialization
 * @retval      -EIO    Initialization error (Series 0/1 only)
 */
int coretemp_init(void);

/**
 * @brief   Read the current temperature from the sensor.
 *
 * The temperature readings are compensated using the factory-calibration
 * values. On Series 0 and Series 1 devices, the sensor value is read via the
 * ADC. On Series 2 devices, the sensor value is read from the EMU peripheral.
 *
 * @retval      -10000  Invalid reading (missing factory calibration data)
 *
 * @return      current temperature in centi-degrees Celsius (times 100)
 */
int16_t coretemp_read(void);

#ifdef __cplusplus
}
#endif

/** @} */
