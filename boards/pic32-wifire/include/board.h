/*
 * Copyright(C) 2017, Imagination Technologies Limited and/or its
 *                 affiliated group companies.
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 *
 */

/**
 * @ingroup     boards_pic32-wifire
 * @details
 * See:
 * http://store.digilentinc.com/chipkit-wi-fire-wifi-enabled-mz-microcontroller-board/
 * for more information on the board.
 *
 * @{
 *
 * @file
 * @brief       board configuration for the Digilent PIC32 WiFire
 *
 * @author      Neil Jones <Neil.Jones@imgtec.com>
 */

#ifndef BOARD_H
#define BOARD_H

#include "periph_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

#include "vendor/p32mz2048efg100.h"

/**
 * @brief   Set how many increments of the count register per uS
 *          needed by the timer code.
 */
#define TICKS_PER_US (100)

/**
 * @brief   We are using an External Interrupt Controller (all pic32 devices use this mode)
 */
#define EIC_IRQ      (1)

/**
 * @name    LED pin configuration
 * @{
 */
#define LED1_PIN            GPIO_PIN(PORT_G, 6)
#define LED2_PIN            GPIO_PIN(PORT_D, 4)
#define LED3_PIN            GPIO_PIN(PORT_B, 11)
#define LED4_PIN            GPIO_PIN(PORT_G, 15)

#define LED1_MASK           (1 << 6)
#define LED2_MASK           (1 << 4)
#define LED3_MASK           (1 << 11)
#define LED4_MASK           (1 << 15)

#define LED1_ON             (LATGSET = LED1_MASK)
#define LED1_OFF            (LATGCLR = LED1_MASK)
#define LED1_TOGGLE         (LATGINV = LED1_MASK)

#define LED2_ON             (LATDSET = LED2_MASK)
#define LED2_OFF            (LATDCLR = LED2_MASK)
#define LED2_TOGGLE         (LATDINV = LED2_MASK)

#define LED3_ON             (LATBSET = LED3_MASK)
#define LED3_OFF            (LATBCLR = LED3_MASK)
#define LED3_TOGGLE         (LATBINV = LED3_MASK)

#define LED4_ON             (LATGSET = LED4_MASK)
#define LED4_OFF            (LATGCLR = LED4_MASK)
#define LED4_TOGGLE         (LATGINV = LED4_MASK)
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
