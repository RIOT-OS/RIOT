/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_led Control on-board LEDs
 * @ingroup     drivers_actuators
 * @brief       Access macros to control the on-board LEDs
 *
 * This header contains a set of macros for controlling the on-board LEDs of
 * a board. The LEDs are enumerated, starting from LED0 to LED7. As most
 * platforms have a different number of LEDs, the existing ones are mapped onto
 * the lowest LED numbers, while the higher LED numbers will simply be empty
 * defines. This ensures, that the LED macros are portable to any platform with
 * any number of LEDs.
 *
 * Providing access macros to 8 LEDs is a random decision, as currently 8 is the
 * maximum number of on-board LEDs found on any board in RIOT (stm32f3discovery).
 *
 * @{
 *
 * @file
 * @brief       Macros for controlling the on-board LEDs
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef LED_H
#define LED_H

#include "board.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    LED fallback macros
 * @{
 */
#ifndef LED0_ON
#define LED0_ON             /**< defined empty */
#define LED0_OFF            /**< defined empty */
#define LED0_TOGGLE         /**< defined empty */
#endif

#ifndef LED1_ON
#define LED1_ON             /**< defined empty */
#define LED1_OFF            /**< defined empty */
#define LED1_TOGGLE         /**< defined empty */
#endif

#ifndef LED2_ON
#define LED2_ON             /**< defined empty */
#define LED2_OFF            /**< defined empty */
#define LED2_TOGGLE         /**< defined empty */
#endif

#ifndef LED3_ON
#define LED3_ON             /**< defined empty */
#define LED3_OFF            /**< defined empty */
#define LED3_TOGGLE         /**< defined empty */
#endif

#ifndef LED4_ON
#define LED4_ON             /**< defined empty */
#define LED4_OFF            /**< defined empty */
#define LED4_TOGGLE         /**< defined empty */
#endif

#ifndef LED5_ON
#define LED5_ON             /**< defined empty */
#define LED5_OFF            /**< defined empty */
#define LED5_TOGGLE         /**< defined empty */
#endif

#ifndef LED6_ON
#define LED6_ON             /**< defined empty */
#define LED6_OFF            /**< defined empty */
#define LED6_TOGGLE         /**< defined empty */
#endif

#ifndef LED7_ON
#define LED7_ON             /**< defined empty */
#define LED7_OFF            /**< defined empty */
#define LED7_TOGGLE         /**< defined empty */
#endif
/** @} */

/**
 * @name    Convenience LED control macros
 * @{
 */
#define LED_ON(x)           LED ## x ##_ON      /**< Turn on led x */
#define LED_OFF(x)          LED ## x ## _OFF    /**< Turn off led x */
#define LED_TOGGLE(x)       LED ## x ##_TOGGLE  /**< Toggle led x */
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* LED_H */
/** @} */
