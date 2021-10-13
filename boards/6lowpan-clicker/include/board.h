/*
 * Copyright(C) 2016,2017, Imagination Technologies Limited and/or its
 *                 affiliated group companies.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @ingroup     boards_6lowpan-clicker
 * @details
 * see:
 * https://www.mikroe.com/clicker-6lowpan
 * For more information on the board.
 *
 * @{
 *
 * @file
 * @brief       board configuration for the MikroE 6LoWPAN Clicker
 *
 * @author      Neil Jones <Neil.Jones@imgtec.com>
 */

#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"
#include "periph_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Set how many increments of the count register per uS
 *          needed by the timer code.
 */
#define TICKS_PER_US (48)

/**
 * @name    LED pin configuration
 * @{
 */
#define LED1_PIN            GPIO_PIN(PORT_B, 1)
#define LED2_PIN            GPIO_PIN(PORT_B, 2)

#define LED1_MASK           (1 << 1)
#define LED2_MASK           (1 << 2)

#define LED1_ON             (LATBSET = LED1_MASK)
#define LED1_OFF            (LATBCLR = LED1_MASK)
#define LED1_TOGGLE         (LATBINV = LED1_MASK)

#define LED2_ON             (LATBSET = LED2_MASK)
#define LED2_OFF            (LATBCLR = LED2_MASK)
#define LED2_TOGGLE         (LATBINV = LED2_MASK)
/** @} */

/**
 * @name    Button pin configuration
 * @{
 */
#define BTN0_PIN            GPIO_PIN(PORT_E, 7)
#define BTN0_MODE           GPIO_IN

#define BTN1_PIN            GPIO_PIN(PORT_B, 0)
#define BTN1_MODE           GPIO_IN
/** @} */

/**
 * @brief   Board level initialization
 */
void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */
