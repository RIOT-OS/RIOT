/*
 * Copyright (C) 2014 ELL-i co-operative
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @defgroup    board_ellduino
 * @ingroup     boards
 * @brief       Support for the Ellduino board
 * @{
 *
 * @file
 * @brief       Board specific definitions for the ELL-i Ellduino board.
 *
 * @author      Lari Lehtomäki <lari@lehtomaki.fi>
 */

#ifndef __BOARD_H
#define __BOARD_H

#include "cpu.h"


/**
 * @name The nominal CPU core clock in this board
 */
#define F_CPU               (48000000UL)

/**
 * @name Assign the peripheral timer to be used as hardware timer
 */
#define HW_TIMER            TIMER_0

/**
 * @name Assign the UART interface to be used for stdio
 */
#define STDIO               UART_0


/**
 * @name Dummy LED definition for startup
 * STM32F0 startup assumes we would have LEDs on board.
 */
#define LD4_TOGGLE          /* not available */

/**
 * @brief Initialize board specific hardware, including clock, LEDs and std-IO
 */
void board_init(void);

#endif /** __BOARD_H */
/** @} */
