/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 *
 * @author Phuong Dang <kamejoko80@yahoo.com>
 */

/**
 * @defgroup    boards_jellyfish JF4418
 * @ingroup     boards
 * @brief       Support for the Jellyfish JF4418 board
 * @{
 *
 * @file
 * @brief       Board specific definitions for the STM32F0Discovery evaluation board.
 *
 * @author      Phuong Dang <kamejoko80@yahoo.com>
 */

#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"

#ifdef __cplusplus
extern "C" {
#endif

#define PORTA (0)
#define PORTB (1)
#define PORTC (2)
#define PORTD (3)
#define PORTE (4)

#define MRF24J40_PARAMS_BOARD  { .spi = 0, \
                                 .spi_clk = MRF24J40_PARAM_SPI_CLK, \
                                 .cs_pin = GPIO_PIN(PORTC, 30), \
                                 .int_pin = GPIO_PIN(PORTB, 31), \
                                 .reset_pin = GPIO_PIN(PORTB, 30) }


/**
 * @brief Initialize board specific hardware, including clock, LEDs and std-IO
 */
void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */
