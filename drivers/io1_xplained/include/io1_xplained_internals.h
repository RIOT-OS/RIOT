/*
 * Copyright (C) 2016 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_io1_xplained
 * @brief       Internal addresses, constants for the IO1 Xplained extension.
 * @{
 *
 * @file
 * @brief       Internal addresses, constants for the IO1 Xplained extension.
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef IO1_XPLAINED_INTERNALS_H
#define IO1_XPLAINED_INTERNALS_H

#include "cpu.h"
#include "periph_cpu.h"
#include "periph/adc.h"
#include "periph/gpio.h"
#include "periph/spi.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    IO1 Xplained I2C addresses
 * @{
 */
#define IO1_TEMPERATURE_BASE_ADDR          (0x48)
#define IO1_TEMPERATURE_DEFAULT_ADDR       (0x07)
/** @} */

/**
 * @name    IO1 Xplained SD Card configuration
 * @{
 */
#define IO1_SDCARD_SPI_PARAM_SPI           SPI_DEV(1)
#define IO1_SDCARD_SPI_PARAM_CS            GPIO_PIN(1, 3)
#define IO1_SDCARD_SPI_PARAM_CLK           GPIO_PIN(1, 23)
#define IO1_SDCARD_SPI_PARAM_MOSI          GPIO_PIN(1, 22)
#define IO1_SDCARD_SPI_PARAM_MISO          GPIO_PIN(1, 2)
#define IO1_SDCARD_SPI_PARAM_POWER         (GPIO_UNDEF)
#define IO1_SDCARD_SPI_PARAM_POWER_AH      (true)
#define IO1_SDCARD_SPI_PARAM_DETECT        GPIO_PIN(0, 23)
/** @} */

/**
 * @name    IO1 Xplained Light sensor ADC configuration
 * @{
 */
#define IO1_LIGHT_ADC_LINE                 ADC_LINE(0)
#define IO1_LIGHT_ADC_RES                  ADC_RES_10BIT
/** @} */

/**
 * @name    IO1 Xplained LED pin
 * @{
 */
#define IO1_LED_PIN                    GPIO_PIN(0, 18)
/** @} */

/**
 * @name    IO1 Xplained gpio pins
 * @{
 */
#define IO1_GPIO1_PIN                  GPIO_PIN(0, 13)
#define IO1_GPIO2_PIN                  GPIO_PIN(0, 28)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* IO1_XPLAINED_INTERNALS_H */
/** @} */
