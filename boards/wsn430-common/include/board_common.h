/*
 * Copyright (C) 2013 Milan Babel <babel@inf.fu-berlin.de>
 *               2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    boards_wsn430-common WSN430 common
 * @ingroup     boards
 * @brief       Common files for WSN430 based boards
 * @{
 *
 * @file
 * @brief       Common definitions for WSN430 based boards
 *
 * @author      Milan Babel <babel@inf.fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 *
 */

#ifndef BOARD_COMMON_H
#define BOARD_COMMON_H

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Address of the info memory
 */
#define INFOMEM     (0x1000)

/**
 * @brief   Xtimer configuration
 * @{
 */
#define XTIMER_DEV                  (0)
#define XTIMER_CHAN                 (0)
#define XTIMER_WIDTH                (16)
#define XTIMER_BACKOFF              (40)
/** @} */

/**
 * @brief   LED pin definitions and handlers
 * @{
 */
#define LED0_PIN                    GPIO_PIN(4, 4)
#define LED1_PIN                    GPIO_PIN(4, 5)
#define LED2_PIN                    GPIO_PIN(4, 6)

#define LED_DIR_REG                 P5DIR
#define LED0_MASK                   (1 << 4)
#define LED1_MASK                   (1 << 5)
#define LED2_MASK                   (1 << 6)

#define LED0_ON                     (LED_DIR_REG &=~LED0_MASK)
#define LED0_OFF                    (LED_DIR_REG |= LED0_MASK)
#define LED0_TOGGLE                 (LED_DIR_REG ^= LED0_MASK)

#define LED1_ON                     (LED_DIR_REG &=~LED1_MASK)
#define LED1_OFF                    (LED_DIR_REG |= LED1_MASK)
#define LED1_TOGGLE                 (LED_DIR_REG ^= LED1_MASK)

#define LED2_ON                     (LED_DIR_REG &=~LED2_MASK)
#define LED2_OFF                    (LED_DIR_REG |= LED2_MASK)
#define LED2_TOGGLE                 (LED_DIR_REG ^= LED2_MASK)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* BOARD_COMMON_H */
/** @} */
