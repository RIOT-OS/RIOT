/*
 * SPDX-FileCopyrightText: 2014 Freie Universität Berlin
 * SPDX-FileCopyrightText: 2015 Zolertia SL
 * SPDX-License-Identifier: LGPL-2.1-only
 */

#pragma once

/**
 * @ingroup     boards_remote-pa
 * @{
 *
 * @file
 * @brief       Board specific definitions for the Re-Mote board prototype A
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Antonio Lignan <alinan@zolertia.com>
 * @author      Sebastian Meiling <s@mlng.net>
 */

#include "cpu.h"
#include "board_common.h"

#ifdef __cplusplus
 extern "C" {
#endif

/**
 * @name    LED pin definitions and handlers
 * @{
 */
#define LED0_PIN            GPIO_PIN(3, 2)
#define LED1_PIN            GPIO_PIN(3, 5)
#define LED2_PIN            GPIO_PIN(2, 3)

#define LED0_MASK           (1 << 2)
#define LED1_MASK           (1 << 5)
#define LED2_MASK           (1 << 3)

#define LED0_ON             (GPIO_D->DATA &= ~LED0_MASK)
#define LED0_OFF            (GPIO_D->DATA |=  LED0_MASK)
#define LED0_TOGGLE         (GPIO_D->DATA ^=  LED0_MASK)

#define LED1_ON             (GPIO_D->DATA &= ~LED1_MASK)
#define LED1_OFF            (GPIO_D->DATA |=  LED1_MASK)
#define LED1_TOGGLE         (GPIO_D->DATA ^=  LED1_MASK)

#define LED2_ON             (GPIO_C->DATA &= ~LED2_MASK)
#define LED2_OFF            (GPIO_C->DATA |=  LED2_MASK)
#define LED2_TOGGLE         (GPIO_C->DATA ^=  LED2_MASK)
/** @} */

/**
 * @name    User button pin definition
 * @{
 */
#define BTN0_PIN            GPIO_PIN(0, 3)
#define BTN0_MODE           GPIO_IN_PU
/** @} */

/**
 * @name    Radio configuration
 *
 * 2.4GHz RF switch controlled by SW
 * @{
 */
 #define RF_SWITCH_GPIO      GPIO_PIN(3, 4)
 #define RF_SWITCH_SUB_GHZ   gpio_set(RF_SWITCH_GPIO)
 #define RF_SWITCH_2_4_GHZ   gpio_clear(RF_SWITCH_GPIO)
 #define RF_SWITCH_TOGGLE    gpio_toggle(RF_SWITCH_GPIO)
/** @} */

#ifdef __cplusplus
} /* end extern "C" */
#endif
/** @} */
