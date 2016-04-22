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

#include "cpu.h"

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
#define XTIMER_BACKOFF              (40)
/** @} */

/**
 * @brief   LED pin definitions and handlers
 * @{
 */
#define LED0_PIN                    GPIO_PIN(4, 7)

#define LED_OUT_REG                 (P5OUT)
#define LED0_MASK                   (1 << 7)

#define LED0_ON                     (LED_OUT_REG &= ~LED0_MASK)
#define LED0_OFF                    (LED_OUT_REG |=  LED0_MASK)
#define LED0_TOGGLE                 (LED_OUT_REG ^=  LED0_MASK)
/** @} */

#ifdef __cplusplus
}
#endif

#endif /* BOARD-CONF_H */
/** @} */
