/*
 * Copyright (C) 2017 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_periph_led LED
 * @ingroup     drivers_periph
 * @brief       Low-level LED peripheral driver
 *
 *
 * @{
 * @file
 * @brief       Low-level LED peripheral driver interface definitions
 *
 * @author      Sebastian Meiling <s@mlng.net>
 */

#ifndef PERIPH_LED_H
#define PERIPH_LED_H

#include "periph/gpio.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Util macros to turn on and off, or toggle on-board LEDs
 *
 * @ {
 */
#ifdef LED0_PIN
#ifndef LED0_ON
/* assuming LED0_OFF and LED0_TOGGLE are undefined too */
#define LED0_ON                 gpio_set(LED0_PIN)
#define LED0_OFF                gpio_clear(LED0_PIN)
#define LED0_TOGGLE             gpio_toggle(LED0_PIN)
#endif /* LED0_ON */
#ifndef LED0_INIT
#define LED0_INIT               do { gpio_init(LED0_PIN, GPIO_OUT); LED0_OFF; } while(0)
#endif /* LED0_INIT */
#endif /* LED0_PIN */

#ifdef LED1_PIN
#ifndef LED1_ON
/* assuming LED1_OFF and LED1_TOGGLE are undefined too */
#define LED1_ON                 gpio_set(LED1_PIN)
#define LED1_OFF                gpio_clear(LED1_PIN)
#define LED1_TOGGLE             gpio_toggle(LED1_PIN)
#endif /* LED1_ON */
#ifndef LED1_INIT
#define LED1_INIT               do { gpio_init(LED1_PIN, GPIO_OUT); LED1_OFF; } while(0)
#endif /* LED1_INIT */
#endif /* LED1_PIN */

#ifdef LED2_PIN
#ifndef LED2_ON
/* assuming LED2_OFF and LED2_TOGGLE are undefined too */
#define LED2_ON                 gpio_set(LED2_PIN)
#define LED2_OFF                gpio_clear(LED2_PIN)
#define LED2_TOGGLE             gpio_toggle(LED2_PIN)
#endif /* LED2_ON */
#ifndef LED2_INIT
#define LED2_INIT               do { gpio_init(LED2_PIN, GPIO_OUT); LED2_OFF; } while(0)
#endif /* LED2_INIT */
#endif /* LED2_PIN */

#ifdef LED3_PIN
#ifndef LED3_ON
/* assuming LED3_OFF and LED3_TOGGLE are undefined too */
#define LED3_ON                 gpio_set(LED3_PIN)
#define LED3_OFF                gpio_clear(LED3_PIN)
#define LED3_TOGGLE             gpio_toggle(LED3_PIN)
#endif /* LED3_ON */
#ifndef LED3_INIT
#define LED3_INIT               do { gpio_init(LED3_PIN, GPIO_OUT); LED3_OFF; } while(0)
#endif /* LED3_INIT */
#endif /* LED3_PIN */

#ifdef LED4_PIN
#ifndef LED4_ON
/* assuming LED4_OFF and LED4_TOGGLE are undefined too */
#define LED4_ON                 gpio_set(LED4_PIN)
#define LED4_OFF                gpio_clear(LED4_PIN)
#define LED4_TOGGLE             gpio_toggle(LED4_PIN)
#endif /* LED4_ON */
#ifndef LED4_INIT
#define LED4_INIT               do { gpio_init(LED4_PIN, GPIO_OUT); LED4_OFF; } while(0)
#endif /* LED4_INIT */
#endif /* LED4_PIN */

#ifdef LED5_PIN
#ifndef LED5_ON
/* assuming LED5_OFF and LED5_TOGGLE are undefined too */
#define LED5_ON                 gpio_set(LED5_PIN)
#define LED5_OFF                gpio_clear(LED5_PIN)
#define LED5_TOGGLE             gpio_toggle(LED5_PIN)
#endif /* LED5_ON */
#ifndef LED5_INIT
#define LED5_INIT               do { gpio_init(LED5_PIN, GPIO_OUT); LED5_OFF; } while(0)
#endif /* LED5_INIT */
#endif /* LED5_PIN */

#ifdef LED6_PIN
#ifndef LED6_ON
/* assuming LED6_OFF and LED6_TOGGLE are undefined too */
#define LED6_ON                 gpio_set(LED6_PIN)
#define LED6_OFF                gpio_clear(LED6_PIN)
#define LED6_TOGGLE             gpio_toggle(LED6_PIN)
#endif /* LED6_ON */
#ifndef LED6_INIT
#define LED6_INIT               do { gpio_init(LED6_PIN, GPIO_OUT); LED6_OFF; } while(0)
#endif /* LED6_INIT */
#endif /* LED6_PIN */

#ifdef LED7_PIN
#ifndef LED7_ON
/* assuming LED7_OFF and LED7_TOGGLE are undefined too */
#define LED7_ON                 gpio_set(LED7_PIN)
#define LED7_OFF                gpio_clear(LED7_PIN)
#define LED7_TOGGLE             gpio_toggle(LED7_PIN)
#endif /* LED7_ON */
#ifndef LED7_INIT
#define LED7_INIT               do { gpio_init(LED7_PIN, GPIO_OUT); LED7_OFF; } while(0)
#endif /* LED7_INIT */
#endif /* LED7_PIN */
/** @} */

/**
 * @brief Init available on-board LEDs and set to off
 */
void led_init(void);

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_LED_H */
/** @} */
