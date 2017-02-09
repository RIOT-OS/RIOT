/*
 * Copyright (C) 2017  Inria
 * Copyright (C) 2015  Freie Universität Berlin
 * Copyright (C) 2015 Hamburg University of Applied Sciences
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    boards_nucleo-f302 Nucleo-F302
 * @ingroup     boards
 * @brief       Board specific files for the nucleo-f302 board
 * @{
 *
 * @file
 * @brief       Board specific definitions for the nucleo-f302 board
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 * @author      Katja Kirstein <katja.kirstein@haw-hamburg.de>
 */

#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"
#include "periph_conf.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   LED pin definitions and handlers
 * @{
 */
#define LED0_PIN            GPIO_PIN(PORT_B, 13)
#define LED0_MASK           (1 << 13)

#define LED0_ON             (GPIOB->BSRR = LED0_MASK)
#define LED0_OFF            (GPIOB->BSRR = (LED0_MASK << 16))
#define LED0_TOGGLE         (GPIOB->ODR     ^= LED0_MASK)
/** @} */

/**
 * @brief   User button
 */
#define BTN_B1_PIN          GPIO_PIN(PORT_C, 13)

/**
 * @brief   Initialize board specific hardware, including clock, LEDs and std-IO
 */
void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */
