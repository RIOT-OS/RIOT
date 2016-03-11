/*
 * Copyright (C) 2013-2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    boards_msb430-common MSB-430 common
 * @ingroup     boards
 * @brief       Common files for all MSB-430 boards
 * @{
 *
 * @file
 * @brief       Common definitions for all msb-430 based boards
 *
 * @author      unknown
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef BOARD_COMMON_H_
#define BOARD_COMMON_H_

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
#define XTIMER                      (0)
#define XTIMER_CHAN                 (0)
#define XTIMER_MASK                 (0xffff0000)
#define XTIMER_SHIFT_ON_COMPARE     (4)
#define XTIMER_BACKOFF              (40)
/** @} */

/**
 * @brief   LED definitions
 * @{
 */
#define LEDS_PxDIR                  (P5DIR)
#define LEDS_PxOUT                  (P5OUT)
#define LEDS_CONF_RED               (0x80)
#define LEDS_CONF_GREEN             (0x00)
#define LEDS_CONF_YELLOW            (0x00)

#define LED_RED_ON                  (LEDS_PxOUT &=~LEDS_CONF_RED)
#define LED_RED_OFF                 (LEDS_PxOUT |= LEDS_CONF_RED)
#define LED_RED_TOGGLE              (LEDS_PxOUT ^= LEDS_CONF_RED)
#define LED_GREEN_ON                /* not present */
#define LED_GREEN_OFF               /* not present */
#define LED_GREEN_TOGGLE            /* not present */
/** @} */


#ifdef __cplusplus
}
#endif

#endif /* BOARD-CONF_H */
/** @} */
