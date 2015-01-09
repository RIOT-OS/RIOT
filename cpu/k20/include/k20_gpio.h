/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     cpu_k20
 * @{
 *
 * @file
 * @brief       K20 specific GPIO definitions
 *
 * @author      Finn Wilke <finn.wilke@fu-berlin.de>
 *
 * @}
 */


#ifndef __K20_GPIO_H
#define __K20_GPIO_H

#ifdef __cplusplus
extern "C" {
#endif

#include "MK20DZ10.h"

/**
 * @brief Enables a GPIO as output
 *
 * @see k20_gpio_enable_input(GPIO_MemMapPtr, uint8_t)
 * @see k20_gpio_direction(GPIO_MemMapPtr, uint8_t, bool)
 *
 * @param[in]  gpio         A pointer to the GPIO. Obtain with the macro PORTx_BASE_PTR
 * @param[in]  pinid        The number of the pin to be set. Must be between 0 and 31
 * @param[in]  high_power   Use high power mode (for driving LEDs and similar peripherals)
 *
 * @return 0 if setting the GPIO was successful, -1 otherwise.
 */
int k20_gpio_enable_output(GPIO_MemMapPtr gpio, uint8_t pinid, bool high_power);

/**
 * @brief Enables a GPIO as input
 *
 * @see k20_gpio_enable_output(GPIO_MemMapPtr, uint8_t, bool)
 * @see k20_gpio_direction(GPIO_MemMapPtr, uint8_t, bool)
 *
 * @param[in]  gpio         A pointer to the GPIO. Obtain with the macro PORTx_BASE_PTR
 * @param[in]  pinid        The number of the pin to be set. Must be between 0 and 31
 *
 * @return 0 if setting the GPIO was successful, -1 otherwise.
 */
int k20_gpio_enable_input(GPIO_MemMapPtr gpio, uint8_t pinid);

/**
 * @brief Reconfigures a GPIO as input/output
 *
 * @see k20_gpio_enable_output(GPIO_MemMapPtr, uint8_t, bool)
 * @see k20_gpio_enable_input(GPIO_MemMapPtr, uint8_t)
 *
 * @param[in]  gpio         A pointer to the GPIO. Obtain with the macro PORTx_BASE_PTR
 * @param[in]  pinid        The number of the pin to be set. Must be between 0 and 31
 * @param[in]  output       Evaluates to true if reconfiguring as output, false if
 *                          reconfiguring as input
 *
 * @return 0 if setting the GPIO was successful, -1 otherwise.
 */
int k20_gpio_direction(GPIO_MemMapPtr gpio, uint8_t pinid, bool output);

#ifdef __cplusplus
}
#endif

#endif /* __K20_GPIO_H */
