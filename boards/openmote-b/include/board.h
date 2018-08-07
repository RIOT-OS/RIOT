/*
 * Copyright (C) 2014 Freie Universität Berlin
 * Copyright (C) 2018 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    boards_openmote-b OpenMote-B
 * @ingroup     boards
 * @brief       Support for the OpenMote-B board
 * @{
 *
 * @file
 * @brief       Board specific definitions for the OpenMote-B board
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Kevin Weiss <kevin.weiss@haw-hamburg.de>
 */

#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"
#include "periph/gpio.h"

#ifdef __cplusplus
 extern "C" {
#endif

/**
 * @name    LED pin definitions and handlers
 * @{
 */
#define LED0_PIN                    GPIO_PIN(2, 4)
#define LED1_PIN                    GPIO_PIN(2, 7)
#define LED2_PIN                    GPIO_PIN(2, 6)
#define LED3_PIN                    GPIO_PIN(2, 5)
#define LED3_PIN                    GPIO_PIN(2, 5)
#define USER_BUTTON_PIN             GPIO_PIN(2, 5)
#define RF_SWITCH_2_4_GHZ_PIN       GPIO_PIN(3, 4)  /**< PD4 -- 2.4ghz */
#define RF_SWITCH_SUB_GHZ_PIN       GPIO_PIN(3, 3)  /**< PD3 -- subghz */

#define LED_PORT                    GPIO_C
#define LED0_MASK                   (1 << 4)
#define LED1_MASK                   (1 << 7)
#define LED2_MASK                   (1 << 6)
#define LED3_MASK                   (1 << 5)

#define RF_SWITCH_PORT              GPIO_D
#define RF_SWITCH_2_4_GHZ_MASK      (1 << 4)
#define RF_SWITCH_SUB_GHZ_MASK      (1 << 3)

#define LED0_ON                     (LED_PORT->DATA &= ~LED0_MASK)
#define LED0_OFF                    (LED_PORT->DATA |=  LED0_MASK)
#define LED0_TOGGLE                 (LED_PORT->DATA ^=  LED0_MASK)

#define LED1_ON                     (LED_PORT->DATA &= ~LED1_MASK)
#define LED1_OFF                    (LED_PORT->DATA |=  LED1_MASK)
#define LED1_TOGGLE                 (LED_PORT->DATA ^=  LED1_MASK)

#define LED2_ON                     (LED_PORT->DATA &= ~LED2_MASK)
#define LED2_OFF                    (LED_PORT->DATA |=  LED2_MASK)
#define LED2_TOGGLE                 (LED_PORT->DATA ^=  LED2_MASK)

#define LED3_ON                     (LED_PORT->DATA &= ~LED3_MASK)
#define LED3_OFF                    (LED_PORT->DATA |=  LED3_MASK)
#define LED3_TOGGLE                 (LED_PORT->DATA ^=  LED3_MASK)

#define RF_SWITCH_2_4_GHZ_ON        (RF_SWITCH_PORT->DATA |=  RF_SWITCH_2_4_GHZ_MASK)
#define RF_SWITCH_2_4_GHZ_OFF       (RF_SWITCH_PORT->DATA &= ~RF_SWITCH_2_4_GHZ_MASK)
#define RF_SWITCH_2_4_GHZ_TOGGLE    (RF_SWITCH_PORT->DATA ^=  RF_SWITCH_2_4_GHZ_MASK)

#define RF_SWITCH_SUB_GHZ_ON        (RF_SWITCH_PORT->DATA |=  RF_SWITCH_SUB_GHZ_MASK)
#define RF_SWITCH_SUB_GHZ_OFF       (RF_SWITCH_PORT->DATA &= ~RF_SWITCH_SUB_GHZ_MASK)
#define RF_SWITCH_SUB_GHZ_TOGGLE    (RF_SWITCH_PORT->DATA ^=  RF_SWITCH_SUB_GHZ_MASK)
/** @} */

/**
 * @name    xtimer configuration
 * @{
 */
#define XTIMER_WIDTH        (16)
#define XTIMER_BACKOFF      (50)
#define XTIMER_ISR_BACKOFF  (40)
/** @} */

/**
 * @name    Flash Customer Configuration Area (CCA) parameters
 * @{
 */
#ifndef UPDATE_CCA
#define UPDATE_CCA                (1)
#endif

#define CCA_BACKDOOR_ENABLE       (1)
#define CCA_BACKDOOR_PORT_A_PIN   (6) /**< BSL_BOOT Pin */
#define CCA_BACKDOOR_ACTIVE_LEVEL (0) /**< Active low */
/** @} */

/**
 * @brief   Initialize board specific hardware, including clock, LEDs and std-IO
 */
void board_init(void);

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* BOARD_H */
/** @} */
