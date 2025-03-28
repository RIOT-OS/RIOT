/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#ifndef LED_H
#define LED_H

/**
 * @defgroup    drivers_led Control on-board LEDs
 * @ingroup     drivers_actuators
 * @brief       Access macros and functions to control the on-board LEDs
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
 * @brief       Macros and inline functions for controlling the on-board LEDs
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

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
#else
#define LED0_IS_PRESENT     /**< indicate that LED0 is present */
#endif

#ifndef LED1_ON
#define LED1_ON             /**< defined empty */
#define LED1_OFF            /**< defined empty */
#define LED1_TOGGLE         /**< defined empty */
#else
#define LED1_IS_PRESENT     /**< indicate that LED1 is present */
#endif

#ifndef LED2_ON
#define LED2_ON             /**< defined empty */
#define LED2_OFF            /**< defined empty */
#define LED2_TOGGLE         /**< defined empty */
#else
#define LED2_IS_PRESENT     /**< indicate that LED2 is present */
#endif

#ifndef LED3_ON
#define LED3_ON             /**< defined empty */
#define LED3_OFF            /**< defined empty */
#define LED3_TOGGLE         /**< defined empty */
#else
#define LED3_IS_PRESENT     /**< indicate that LED3 is present */
#endif

#ifndef LED4_ON
#define LED4_ON             /**< defined empty */
#define LED4_OFF            /**< defined empty */
#define LED4_TOGGLE         /**< defined empty */
#else
#define LED4_IS_PRESENT     /**< indicate that LED4 is present */
#endif

#ifndef LED5_ON
#define LED5_ON             /**< defined empty */
#define LED5_OFF            /**< defined empty */
#define LED5_TOGGLE         /**< defined empty */
#else
#define LED5_IS_PRESENT     /**< indicate that LED5 is present */
#endif

#ifndef LED6_ON
#define LED6_ON             /**< defined empty */
#define LED6_OFF            /**< defined empty */
#define LED6_TOGGLE         /**< defined empty */
#else
#define LED6_IS_PRESENT     /**< indicate that LED6 is present */
#endif

#ifndef LED7_ON
#define LED7_ON             /**< defined empty */
#define LED7_OFF            /**< defined empty */
#define LED7_TOGGLE         /**< defined empty */
#else
#define LED7_IS_PRESENT     /**< indicate that LED7 is present */
#endif

/**
 * Number of LEDs available on the current board.
 */
#if defined(LED7_IS_PRESENT)
#define LED_NUMOF 8
#elif defined(LED6_IS_PRESENT)
#define LED_NUMOF 7
#elif defined(LED5_IS_PRESENT)
#define LED_NUMOF 6
#elif defined(LED4_IS_PRESENT)
#define LED_NUMOF 5
#elif defined(LED3_IS_PRESENT)
#define LED_NUMOF 4
#elif defined(LED2_IS_PRESENT)
#define LED_NUMOF 3
#elif defined(LED1_IS_PRESENT)
#define LED_NUMOF 2
#elif defined(LED0_IS_PRESENT)
#define LED_NUMOF 1
#else
#define LED_NUMOF 0
#endif

/** @} */

/**
 * @name    Convenience LED control functions and macros
 * @{
 */
#define LED_ON(id)           LED ## id ##_ON      /**< Turn on an LED */
#define LED_OFF(id)          LED ## id ## _OFF    /**< Turn off an LED */
#define LED_TOGGLE(id)       LED ## id ##_TOGGLE  /**< Toggle an LED */

/**
 * Turn on an LED.
 *
 * @note If id is a compile-time constant, consider using @ref LED_ON(id) instead.
 *
 * @param id    id of LED between 0 and 7
 */
static inline void led_on(unsigned id)
{
    switch (id) {
        case 0: LED0_ON; break;
        case 1: LED1_ON; break;
        case 2: LED2_ON; break;
        case 3: LED3_ON; break;
        case 4: LED4_ON; break;
        case 5: LED5_ON; break;
        case 6: LED6_ON; break;
        case 7: LED7_ON; break;
    }
}

/**
 * Turn off an LED.
 *
 * @note If id is a compile-time constant, consider using @ref LED_OFF(id) instead.
 *
 * @param id    id of LED between 0 and 7
 */
static inline void led_off(unsigned id)
{
    switch (id) {
        case 0: LED0_OFF; break;
        case 1: LED1_OFF; break;
        case 2: LED2_OFF; break;
        case 3: LED3_OFF; break;
        case 4: LED4_OFF; break;
        case 5: LED5_OFF; break;
        case 6: LED6_OFF; break;
        case 7: LED7_OFF; break;
    }
}

/**
 * Toggle an LED.
 *
 * @note If id is a compile-time constant, consider using @ref LED_TOGGLE(id) instead.
 *
 * @param id    id of LED between 0 and 7
 */
static inline void led_toggle(unsigned id)
{
    switch (id) {
        case 0: LED0_TOGGLE; break;
        case 1: LED1_TOGGLE; break;
        case 2: LED2_TOGGLE; break;
        case 3: LED3_TOGGLE; break;
        case 4: LED4_TOGGLE; break;
        case 5: LED5_TOGGLE; break;
        case 6: LED6_TOGGLE; break;
        case 7: LED7_TOGGLE; break;
    }
}

/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* LED_H */
