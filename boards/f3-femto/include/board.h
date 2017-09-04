/*
 * Copyright (C) 2017 HAW Hamburg
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     boards_f3-femto
 * @{
 *
 * @file
 * @brief       Board specific definitions for the F3-Femto Flight Controller.
 *
 * @author      Sebastian Meiling <s@mlng.net>
 */

#ifndef BOARD_H
#define BOARD_H

#include <stdint.h>

#include "cpu.h"
#include "periph_conf.h"

#ifdef __cplusplus
 extern "C" {
#endif

/**
 * @brief   Macros for controlling the on-board LED
 * @{
 */
#define LED0_PIN            GPIO_PIN(PORT_B, 8)
#define LED0_PORT           (GPIOB)
#define LED0_MASK           (1 << 8)

#define LED0_ON             (LED0_PORT->BSRR = LED0_MASK)
#define LED0_OFF            (LED0_PORT->BRR  = LED0_MASK)
#define LED0_TOGGLE         (LED0_PORT->ODR ^= LED0_MASK)
/** @} */

/**
 * @brief Initialize board specific hardware, including clock, LEDs and std-IO
 */
void board_init(void);

/**
 * @brief   Use the 2nd UART for STDIO on this board
 */
#define UART_STDIO_DEV      UART_DEV(1)

#ifdef __cplusplus
} /* end extern "C" */
#endif

#endif /* BOARD_H */
/** @} */
