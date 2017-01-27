/*
 * Copyright (C) 2014 Freie Universität Berlin
 * Copyright (C) 2015 Zolertia SL
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    board_remote Re-Mote
 * @ingroup     boards
 * @brief       Support for the Re-Mote board prototype A
 * @{
 *
 * @file
 * @brief       Board specific definitions for the Re-Mote board prototype A
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *              Antonio Lignan <alinan@zolertia.com>
 */

#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"
#include "board_common.h"

#ifdef __cplusplus
 extern "C" {
#endif

/**
 * @brief   LED pin definitions and handlers
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
 * @name User button pin definition
 * @{
 */
#define BTN0_PIN            GPIO_PIN(0, 3)
/** @} */

/**
 * @name 2.4GHz RF switch controlled by SW
 * @{
 */
#define RF_SWITCH_PORT      GPIO_D
#define RF_SWITCH_PIN       (4)
#define RF_SWITCH_EXTERNAL  (RF_SWITCH_PORT->DATA |= (1 << RF_SWITCH_PIN))
#define RF_SWITCH_INTERNAL  (RF_SWITCH_PORT->DATA &= ~(1 << RF_SWITCH_PIN))
#define RF_SWITCH_TOGGLE    (RF_SWITCH_PORT->DATA ^= (1 << RF_SWITCH_PIN))
/** @} */

/**
 * @brief Initialize board specific hardware, including clock, LEDs and std-IO
 */
void board_init(void);

#ifdef __cplusplus
} /* end extern "C" */
#endif
#endif /* BOARD_H */
/** @} */
