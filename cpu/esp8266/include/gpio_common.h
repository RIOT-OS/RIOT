/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp8266
 * @{
 *
 * @file
 * @brief       Low-level GPIO driver implementation for ESP8266
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @}
 */

#ifndef GPIO_COMMON_H
#define GPIO_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

/** Map of GPIO pin numbers to IOMUX pin numbers */
extern const uint8_t _gpio_to_iomux[];
/** Map of IOMUX pin numbers to GPIO pin numbers */
extern const uint8_t _iomux_to_gpio[];

/**
 * @brief   Definition of possible GPIO usage types
 */
typedef enum
{
    _GPIO = 0,  /**< pin used as standard GPIO */
    _I2C,       /**< pin used as I2C signal */
    _PWM,       /**< pin used as PWM output */
    _SPI,       /**< pin used as SPI interface */
    _SPIF,      /**< pin used as SPI flash interface */
    _UART,      /**< pin used as UART interface */
} _gpio_pin_usage_t;

/**
 * Holds the usage type of each GPIO pin
 */
extern _gpio_pin_usage_t _gpio_pin_usage [GPIO_PIN_NUMOF];

#ifdef __cplusplus
}
#endif

#endif /* GPIO_COMMON_H */
