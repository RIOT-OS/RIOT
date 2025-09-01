/*
 * SPDX-FileCopyrightText: 2017 Ken Rabold
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @defgroup    boards_hifive1 SiFive HiFive1 RISC-V board
 * @ingroup     boards
 * @brief       Support for the SiFive HiFive1 RISC-V board
 * @{
 *
 * @file
 * @brief       Board specific definitions for the SiFive HiFive1 RISC-V board
 *
 * @author      Ken Rabold
 */

#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Xtimer configuration
 * @{
 */
#define XTIMER_HZ                   (32768UL)
/** @} */

/**
 * @name    Macros for controlling the on-board LEDs
 * @{
 */
#define LED0_PIN            GPIO_PIN(0, 22) /* Red */
#define LED1_PIN            GPIO_PIN(0, 19) /* Green */
#define LED2_PIN            GPIO_PIN(0, 21) /* Blue */

#define LED0_ON             gpio_clear(LED0_PIN)
#define LED0_OFF            gpio_set(LED0_PIN)
#define LED0_TOGGLE         gpio_toggle(LED0_PIN)

#define LED1_ON             gpio_clear(LED1_PIN)
#define LED1_OFF            gpio_set(LED1_PIN)
#define LED1_TOGGLE         gpio_toggle(LED1_PIN)

#define LED2_ON             gpio_clear(LED2_PIN)
#define LED2_OFF            gpio_set(LED2_PIN)
#define LED2_TOGGLE         gpio_toggle(LED2_PIN)
/** @} */

/**
 * @brief   Initialize the board clock to use PLL and faster SPI access.
 */
void board_init_clock(void);

#ifdef __cplusplus
}
#endif

/** @} */
