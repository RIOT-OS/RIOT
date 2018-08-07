/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_calliope-mini
 *
 * This board is for calliope-mini revision 1.0.
 * @{
 *
 * @file
 * @brief       Board specific configuration for the Calliope mini
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Sebastian Meiling <s@mlng.net>
 */

#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"
#include "periph_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    Xtimer configuration
 * @{
 */
#define XTIMER_WIDTH                (24)
#define XTIMER_BACKOFF              (40)
/** @} */

/**
 * @name    LED matrix pin configuration
 * @{
 */
#define MINI_LED_COL1               GPIO_PIN(0,  4)
#define MINI_LED_COL2               GPIO_PIN(0,  5)
#define MINI_LED_COL3               GPIO_PIN(0,  6)
#define MINI_LED_COL4               GPIO_PIN(0,  7)
#define MINI_LED_COL5               GPIO_PIN(0,  8)
#define MINI_LED_COL6               GPIO_PIN(0,  9)
#define MINI_LED_COL7               GPIO_PIN(0, 10)
#define MINI_LED_COL8               GPIO_PIN(0, 11)
#define MINI_LED_COL9               GPIO_PIN(0, 12)
#define MINI_LED_ROW1               GPIO_PIN(0, 13)
#define MINI_LED_ROW2               GPIO_PIN(0, 14)
#define MINI_LED_ROW3               GPIO_PIN(0, 15)
/** @} */

/**
 * @name    Button configuration
 * @{
 */
#define BTN0_PIN                    GPIO_PIN(0, 17)
#define BTN0_MODE                   GPIO_IN
#define BTN1_PIN                    GPIO_PIN(0, 16)
#define BTN1_MODE                   GPIO_IN
/** @} */

/**
 * @brief   Initialize the board, also triggers the CPU initialization
 */
void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */
