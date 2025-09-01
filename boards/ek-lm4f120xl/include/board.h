/*
 * SPDX-FileCopyrightText: 2015 Rakendra Thapa <rakendrathapa@gmail.com>
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_ek-lm4f120xl
 * @{
 *
 * @file
 * @brief       Board specific definitions for the Stellaris Launchpad LM4F120 board
 *
 * @author      Rakendra Thapa <rakendrathapa@gmail.com>
 */

#include "cpu.h"
#include "periph/uart.h"
#include "periph/timer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Button pin definitions
 * @{
 */
#define BTN0_PIN            GPIO_PIN(5, 4)
#define BTN1_PIN            GPIO_PIN(5, 0)

#define BTN0_MODE           GPIO_IN_PU
#define BTN1_MODE           GPIO_IN_PU
/** @} */

/**
 * @name    LED pin definitions and handlers
 * @{
 */
#define LED0_PIN            GPIO_PIN(5, 1)
#define LED1_PIN            GPIO_PIN(5, 2)
#define LED2_PIN            GPIO_PIN(5, 3)

/**
 * @brief Port used for `LED0_ON` and similar implementations
 * @internal
 * */
#define LED_PORT()          (GPIO_PORTF_DATA_R)
#define LED0_MASK           (1 << 7)
#define LED1_MASK           (1 << 2)
#define LED2_MASK           (1 << 1)

#define LED0_ON             (LED_PORT() |=  LED0_MASK)
#define LED0_OFF            (LED_PORT() &= ~LED0_MASK)
#define LED0_TOGGLE         (LED_PORT() ^=  LED0_MASK)

#define LED1_ON             (LED_PORT() |=  LED1_MASK)
#define LED1_OFF            (LED_PORT() &= ~LED1_MASK)
#define LED1_TOGGLE         (LED_PORT() ^=  LED1_MASK)

#define LED2_ON             (LED_PORT() |=  LED2_MASK)
#define LED2_OFF            (LED_PORT() &= ~LED2_MASK)
#define LED2_TOGGLE         (LED_PORT() ^=  LED2_MASK)
/** @} */

/**
 * @name    ztimer configuration
 * @{
 */
#define CONFIG_ZTIMER_USEC_TYPE    ZTIMER_TYPE_PERIPH_TIMER
#define CONFIG_ZTIMER_USEC_DEV     TIMER_DEV(0)
#define CONFIG_ZTIMER_USEC_MIN     (8)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
