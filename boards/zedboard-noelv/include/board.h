/*
 * SPDX-FileCopyrightText: 2026 Matvii Ivashchenko
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_zedboard_noelv
 * @{
 *
 * @file
 * @brief       Board specific definitions for the Gaisler NOEL-V ZedBoard
 *
 * @author      Matvii Ivashchenko
 */

#include "cpu.h"
#include "periph_conf.h"
#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    System timer frequency
 * @{
 */
#define CONFIG_ZTIMER_USEC_BASE_FREQ (CLOCK_CORECLOCK / 2) /**< ztimer base = mtime */
/** @} */

/**
 * @name    LED definitions (ZedBoard LD0..LD7)
 *
 * VHDL: gpio_o(i+16) → led(i), for i in 0 to 7.
 * So LED0 = gpio_o[16], ..., LED7 = gpio_o[23].
 * Active HIGH (gpio_set = on).
 * @{
 */
#define LED0_PIN            GPIO_PIN(0, 16) /**< LD0 → gpio_o[16] */
#define LED1_PIN            GPIO_PIN(0, 17) /**< LD1 → gpio_o[17] */
#define LED2_PIN            GPIO_PIN(0, 18) /**< LD2 → gpio_o[18] */
#define LED3_PIN            GPIO_PIN(0, 19) /**< LD3 → gpio_o[19] */
#define LED4_PIN            GPIO_PIN(0, 20) /**< LD4 → gpio_o[20] */
#define LED5_PIN            GPIO_PIN(0, 21) /**< LD5 → gpio_o[21] */
#define LED6_PIN            GPIO_PIN(0, 22) /**< LD6 → gpio_o[22] */
#define LED7_PIN            GPIO_PIN(0, 23) /**< LD7 → gpio_o[23] */

#define LED0_ON             gpio_set(LED0_PIN)
#define LED0_OFF            gpio_clear(LED0_PIN)
#define LED0_TOGGLE         gpio_toggle(LED0_PIN)

#define LED1_ON             gpio_set(LED1_PIN)
#define LED1_OFF            gpio_clear(LED1_PIN)
#define LED1_TOGGLE         gpio_toggle(LED1_PIN)
/** @} */

/**
 * @name    Button definitions (ZedBoard BTND, BTNL, BTNR)
 *
 * VHDL: btn(i) → gpio_i(i+4), for i in 1 to 3.
 * BTN0 (BTNC) is NOT connected to GPIO.
 * BTN1 (BTND) = gpio_i[5], BTN2 (BTNL) = gpio_i[6], BTN3 (BTNR) = gpio_i[7].
 * Active HIGH (pressed = 1).
 * @{
 */
#define BTN1_PIN            GPIO_PIN(0, 5) /**< BTND  → gpio_i[5] */
#define BTN2_PIN            GPIO_PIN(0, 6) /**< BTNL  → gpio_i[6] */
#define BTN3_PIN            GPIO_PIN(0, 7) /**< BTNR  → gpio_i[7] */
#define BTN4_PIN            GPIO_PIN(0, 0) /**< SW0   → gpio_i[0] */
#define BTN5_PIN            GPIO_PIN(0, 1) /**< SW1   → gpio_i[1] */
#define BTN6_PIN            GPIO_PIN(0, 2) /**< SW2   → gpio_i[2] */
#define BTN7_PIN            GPIO_PIN(0, 3) /**< SW3   → gpio_i[3] */

#define BTN1_MODE           GPIO_IN
#define BTN2_MODE           GPIO_IN
#define BTN3_MODE           GPIO_IN
#define BTN4_MODE           GPIO_IN
#define BTN5_MODE           GPIO_IN
#define BTN6_MODE           GPIO_IN
#define BTN7_MODE           GPIO_IN
/** @} */

/**
 * @name    DIP Switch definitions (ZedBoard SW0..SW3)
 *
 * VHDL: sw(i) → gpio_i(i), for i in 0 to 2.
 * SW3 → gpio_i[3] (also controls DSU/UART mux: 1=debug UART, 0=UART0).
 * @{
 */
#define SW0_PIN             GPIO_PIN(0, 0)  /**< SW0 -> gpio_i[0] */
#define SW1_PIN             GPIO_PIN(0, 1)  /**< SW1 -> gpio_i[1] */
#define SW2_PIN             GPIO_PIN(0, 2)  /**< SW2 -> gpio_i[2] */
#define SW3_PIN             GPIO_PIN(0, 3)  /**< SW3 -> gpio_i[3] (UART/DSU mux) */
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
