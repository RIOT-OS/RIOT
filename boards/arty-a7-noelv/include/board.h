/*
 * SPDX-FileCopyrightText: 2026 Matvii Ivashchenko
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_arty-a7-noelv
 * @{
 *
 * @file
 * @brief       Board specific definitions for the Gaisler NOEL-V Arty A7
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
 * @name    LED definitions (Arty A7 LD4..LD7)
 *
 * VHDL: gpio_o(i+16) → led(i), for i in 0 to 3.
 * The four green LEDs LD4..LD7 are connected; the RGB LEDs LD0..LD3 are
 * not wired up in the reference design.
 * So LED0 = LD4 = gpio_o[16], ..., LED3 = LD7 = gpio_o[19].
 * Active HIGH (gpio_set = on).
 * @{
 */
#define LED0_PIN            GPIO_PIN(0, 16) /**< LD4 → gpio_o[16] */
#define LED1_PIN            GPIO_PIN(0, 17) /**< LD5 → gpio_o[17] */
#define LED2_PIN            GPIO_PIN(0, 18) /**< LD6 → gpio_o[18] */
#define LED3_PIN            GPIO_PIN(0, 19) /**< LD7 → gpio_o[19] */

#define LED0_ON             gpio_set(LED0_PIN)
#define LED0_OFF            gpio_clear(LED0_PIN)
#define LED0_TOGGLE         gpio_toggle(LED0_PIN)

#define LED1_ON             gpio_set(LED1_PIN)
#define LED1_OFF            gpio_clear(LED1_PIN)
#define LED1_TOGGLE         gpio_toggle(LED1_PIN)

#define LED2_ON             gpio_set(LED2_PIN)
#define LED2_OFF            gpio_clear(LED2_PIN)
#define LED2_TOGGLE         gpio_toggle(LED2_PIN)

#define LED3_ON             gpio_set(LED3_PIN)
#define LED3_OFF            gpio_clear(LED3_PIN)
#define LED3_TOGGLE         gpio_toggle(LED3_PIN)
/** @} */

/**
 * @name    Button definitions (Arty A7 BTN1..BTN3)
 *
 * VHDL: btn(i) → gpio_i(i+4), for i in 1 to 3.
 * BTN0 is the system reset button and is NOT connected to GPIO.
 * BTN1 = gpio_i[5], BTN2 = gpio_i[6], BTN3 = gpio_i[7].
 * The slide switches SW0..SW3 are additionally exposed as BTN4..BTN7.
 * Active HIGH (pressed = 1).
 * @{
 */
#define BTN1_PIN            GPIO_PIN(0, 5) /**< BTN1  → gpio_i[5] */
#define BTN2_PIN            GPIO_PIN(0, 6) /**< BTN2  → gpio_i[6] */
#define BTN3_PIN            GPIO_PIN(0, 7) /**< BTN3  → gpio_i[7] */
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
 * @name    Slide switch definitions (Arty A7 SW0..SW3)
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
