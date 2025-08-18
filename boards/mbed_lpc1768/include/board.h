/*
 * SPDX-FileCopyrightText: 2014 INRIA
 * SPDX-FileCopyrightText: 2014 Freie Universität Berlin
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_mbed_lpc1768
 * @{
 *
 * @file
 * @brief       Board specific definitions for the mbed_lpc1768 board
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#include "cpu.h"
#include "periph_conf.h"
#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    LED pin definitions and handlers
 * @{
 */
#define LED0_PIN            GPIO_PIN(1, 18)
#define LED1_PIN            GPIO_PIN(1, 20)
#define LED2_PIN            GPIO_PIN(1, 21)
#define LED3_PIN            GPIO_PIN(1, 23)

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

#ifdef __cplusplus
}
#endif

/** @} */
