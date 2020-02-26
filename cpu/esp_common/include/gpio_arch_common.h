/*
 * Copyright (C) 2019 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp_common
 * @{
 *
 * @file
 * @brief       Architecture specific GPIO functions for ESP SoCs
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @}
 */

#ifndef GPIO_ARCH_COMMON_H
#define GPIO_ARCH_COMMON_H

#ifndef DOXYGEN

#ifdef __cplusplus
extern "C" {
#endif

/** Definitions for source code compatibility with ESP-IDF */
#define GPIO_MODE_INPUT         GPIO_IN
#define GPIO_MODE_OUTPUT        GPIO_OUT
#define GPIO_MODE_INPUT_OUTPUT  GPIO_IN_OUT

/**
 * @brief   Definition of possible GPIO usage types (for internal use only)
 */
typedef enum
{
    _GPIO = 0,  /**< pin used as standard GPIO */
#ifdef MCU_ESP32
    _ADC,       /**< pin is used as ADC input */
    _CAN,       /**< pin is used as CAN signal */
    _DAC,       /**< pin is used as DAC output */
    _EMAC,      /**< pin is used as EMAC signal */
#endif /* MCU_ESP32 */
    _I2C,       /**< pin is used as I2C signal */
    _PWM,       /**< pin is used as PWM output */
    _SPI,       /**< pin is used as SPI interface */
    _SPIF,      /**< pin is used as SPI flash interface */
    _UART,      /**< pin is used as UART interface */
    _NOT_EXIST  /**< pin cannot be used at all */
} gpio_pin_usage_t;


/**
 * @brief   Set the usage type of the pin
 * @param   pin     GPIO pin
 * @param   usage   GPIO pin usage type
 * @return  0 on success
 *         -1 on error
 */
int gpio_set_pin_usage(gpio_t pin, gpio_pin_usage_t usage);

/**
 * @brief   Get the usage type of the pin
 * @param   pin     GPIO pin
 * @return  GPIO pin usage type on success
 *          _NOT_EXIST on error
 */
gpio_pin_usage_t gpio_get_pin_usage(gpio_t pin);

/**
 * @brief   Get the usage type of the pin as string
 * @param   pin     GPIO pin
 * @return  GPIO pin usage type string on success
 *          _NOT_EXIST on error
 */
const char* gpio_get_pin_usage_str(gpio_t pin);

#ifdef __cplusplus
}
#endif

#endif /* DOXYGEN */
#endif /* GPIO_ARCH_COMMON_H */
