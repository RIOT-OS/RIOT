/*
 * Copyright (C) 2017 Inria
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    boards_nucleo32-common STM Nucleo-32 Common
 * @ingroup     boards
 * @brief       Common files for STM Nucleo-32 boards
 * @{
 *
 * @file
 * @brief       Common pin definitions and board configuration options
 *
 * @author      Alexandre Abadie <alexandre.abadie@inria.fr>
 */

#ifndef BOARD_COMMON_H
#define BOARD_COMMON_H

#include "cpu.h"
#include "periph_conf.h"
#include "arduino_pinmap.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name Macros for controlling the on-board LED (LD3).
 * @{
 */
#define LED0_PIN            GPIO_PIN(PORT_B, 3)
#define LED0_MASK           (1 << 3)
#define LED0_ON             (GPIOB->BSRR = LED0_MASK)
#define LED0_OFF            (GPIOB->BSRR = (LED0_MASK << 16))
#define LED0_TOGGLE         (GPIOB->ODR  ^= LED0_MASK)
/** @} */

/**
 * @brief   Initialize board specific hardware, including clock, LEDs and std-IO
 */
void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /* BOARD_COMMON_H */
/** @} */
