/*
 * SPDX-FileCopyrightText: 2020 iosabi
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     cpu_qn908x
 *
 * @{
 *
 * @file
 * @brief       Common Pin MUX functions.
 *
 * The pins in this CPU are multiplexed to several different function. This
 * module allows to configure the pin multiplexer (MUX) from peripheral drivers.
 *
 * @author      iosabi <iosabi@protonmail.com>
 */

#include <stdint.h>
#include "periph_cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Obtain the GPIO_BASE address from a GPIO_PIN(x, y) value.
 */
#define GPIO_T_ADDR_BASE(pin) (GPIOA_BASE + ((pin) & 0xf000u))

/**
 * @brief   Obtain the GPIO_BASE GPIO_Type* pointer from a GPIO_PIN(x, y) value.
 */
#define GPIO_T_ADDR(pin) ((GPIO_Type *)(GPIO_T_ADDR_BASE(pin)))

/**
 * @brief   Obtain the "x" port number (0 based) from a GPIO_PIN(x, y) value.
 *
 * This macro needs to be kept in sync with the definition of GPIO_PIN.
 */
#define GPIO_T_PORT(pin) ((gpio_t)(pin) >> 12u)

/**
 * @brief   Obtain the pin number "y" from a GPIO_PIN(x, y) value.
 */
#define GPIO_T_PIN(pin) ((pin) & 0x00ffu)

#if defined(GPIOB_BASE) && (GPIO_T_ADDR_BASE(GPIO_PIN(1, 1)) != GPIOB_BASE)
#error "GPIO_T_ADDR(GPIO_PIN(1, x)) must be the GPIOB address"
#endif

/**
 * @brief   Return whether the given pin is a CSHW pin.
 */
#define GPIO_T_IS_HWCS(pin) (((pin) & 0xff00u) == 0x8000)

/**
 * @brief   Return the given CSHW number from the gpio_t pin.
 */
#define GPIO_T_HWCS(pin) ((pin) & 0x0003u)

/**
 * @brief Configure the pin mux to the given function.
 *
 * The meaning of the function value will depend on the gpio pin.
 */
void gpio_init_mux(gpio_t pin, uint32_t func);

#ifdef __cplusplus
}
#endif

/** @} */
