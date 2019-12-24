/*
 * Copyright (C) 2018 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     cpu_esp32
 * @{
 *
 * @file
 * @brief       Architecture specific GPIO functions for ESP32
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 * @}
 */

#ifndef GPIO_ARCH_H
#define GPIO_ARCH_H

#include "periph/gpio.h"
#include "soc/io_mux_reg.h"
#include "soc/gpio_sig_map.h"

#ifndef DOXYGEN

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Definitions for source code compatibility with ESP-IDF
 */
#define GPIO_MODE_INPUT         GPIO_IN
#define GPIO_MODE_OUTPUT        GPIO_OUT
#define GPIO_MODE_INPUT_OUTPUT  GPIO_IN_OUT
/**
 * @brief   Definition of possible GPIO usage types (for internal use only)
 */
typedef enum
{
    _GPIO = 0,  /**< pin used as standard GPIO */
    _ADC,       /**< pin used as ADC input */
    _CAN,       /**< pin used as CAN signal */
    _DAC,       /**< pin used as DAC output */
    _EMAC,      /**< pin used as EMAC signal */
    _I2C,       /**< pin used as I2C signal */
    _PWM,       /**< pin used as PWM output */
    _SPI,       /**< pin used as SPI interface */
    _SPIF,      /**< pin used as SPI flash interface */
    _UART,      /**< pin used as UART interface */
    _NOT_EXIST  /**< pin cannot be used at all */
} gpio_pin_usage_t;

/**
 * @brief   Table of GPIO to IOMUX register mappings
 */
extern const uint32_t _gpio_to_iomux_reg[];
#define GPIO_PIN_MUX_REG _gpio_to_iomux_reg

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

/**
 * @brief   Disable the pullup of the pin
 */
void gpio_pullup_dis (gpio_t pin);

/**
 * @brief   Returns the RTCIO pin number or -1 if the pin is not an RTCIO pin
 */
int8_t gpio_is_rtcio (gpio_t pin);

/**
 * @brief   Configure sleep mode for an GPIO pin if the pin is an RTCIO pin
 * @param   pin     GPIO pin
 * @param   mode    active in sleep mode if true
 * @param   input   as input if true, as output otherwise
 * @return  0 on success
 * @return -1 on invalid pin
 */
int gpio_config_sleep_mode (gpio_t pin, bool sleep_mode, bool input);

/**
 * @brief   GPIO set direction init the pin calling gpio_init
 * @param   pin     GPIO pin
 * @param   mode    active in sleep mode if true
 * @return  0 on success
 *         -1 on invalid argument
 */
int gpio_set_direction(gpio_t pin, gpio_mode_t mode);

/**
 * @brief   extern declaration of ROM functions to avoid compilation problems
 */
void gpio_matrix_in (uint32_t gpio, uint32_t signal_idx, bool inv);
void gpio_matrix_out(uint32_t gpio, uint32_t signal_idx, bool out_inv, bool oen_inv);

#ifdef __cplusplus
}
#endif

#endif /* DOXYGEN */
#endif /* GPIO_ARCH_H */
