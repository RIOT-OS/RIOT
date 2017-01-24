/*
 * Copyright (C) 2013 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    boards_msba2 MSB-A2
 * @ingroup     boards
 * @brief       Support for the ScatterWeb MSB-A2 board
 * @{
 *
 * @file
 * @brief       Basic definitions for the MSB-A2 board
 *
 * @author      unknown
 */

#ifndef BOARD_H
#define BOARD_H

#include "msba2_common.h"
#include "bitarithm.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   LED pin definitions and handlers
 * @{
 */
#define LED0_PIN            GPIO_PIN(2, 25)
#define LED1_PIN            GPIO_PIN(2, 26)

#define LED0_MASK           (BIT25)
#define LED1_MASK           (BIT26)

#define LED0_OFF            (FIO3SET  = LED0_MASK)
#define LED0_ON             (FIO3CLR  = LED0_MASK)
#define LED0_TOGGLE         (FIO3PIN ^= LED0_MASK)

#define LED1_OFF            (FIO3SET  = LED1_MASK)
#define LED1_ON             (FIO3CLR  = LED1_MASK)
#define LED1_TOGGLE         (FIO3PIN ^= LED1_MASK)
/** @} */

/**
 * @name xtimer tuning values
 * @{
 */
#define XTIMER_OVERHEAD     7
#define XTIMER_SHOOT_EARLY  3
/** @} */

/**
 * @brief initialize the board's clock system
 */
void init_clks1(void);

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */
