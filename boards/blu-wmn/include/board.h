/*
 * Copyright (C) 2014 Freie Universität Berlin, Hinnerk van Bruinehsen
 *               2016 Laurent Navet <laurent.navet@gmail.com>
 *               2018 Matthew Blue <matthew.blue.neuro@gmail.com>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/*
 * @defgroup    boards_blu-wmn BLU-WMN001
 * @ingroup     boards
 * @brief       Support for the BLU-WMN001 board.
 * @{
 *
 * @file
 * @brief       Board specific definitions for the BLU-WMN001 board.
 *
 * @author      Hinnerk van Bruinehsen <h.v.bruinehsen@fu-berlin.de>
 * @author      Laurent Navet <laurent.navet@gmail.com>
 * @author      Matthew Blue <matthew.blue.neuro@gmail.com>
 */

#ifndef BOARD_H
#define BOARD_H

#include "cpu.h"
#include "blu-wmn_pinmap.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @name    STDIO configuration
 *
 * As the CPU is too slow to handle 115200 baud, we set the default
 * baudrate to 9600 for this board
 * @{
 */
#ifndef UART_STDIO_BAUDRATE
#define UART_STDIO_BAUDRATE (9600U)
#endif
/** @} */

/**
 * @brief   Use the UART 0 for STDIO on this board
 */
#define UART_STDIO_DEV       (UART_DEV(1))

/**
 * @brief   Context swap defines
 *
 * Setup to use PC4 which is pin change interrupt 20 (PCINT13)
 * This emulates a software triggered interrupt
 */
#define AVR_CONTEXT_SWAP_INIT do { \
            DDRC |= (1 << PC4); \
            PCICR |= (1 << PCIE2); \
            PCMSK2 |= (1 << PCINT20); \
} while (0)
#define AVR_CONTEXT_SWAP_INTERRUPT_VECT  PCINT2_vect
#define AVR_CONTEXT_SWAP_TRIGGER   PORTC ^= (1 << PC4)

/**
 * @name    xtimer configuration values
 * @{
 */
#define XTIMER_DEV                  (0)
#define XTIMER_CHAN                 (0)
#define XTIMER_WIDTH                (16)
#define XTIMER_HZ                   (125000UL)
#define XTIMER_BACKOFF              (40)
/** @} */

/**
 * @brief   Define the interface to the AT86RF212B / AT86RF231 radio
 */
#define AT86RF2XX_PARAM_CS          WIRELESS_SEL
#define AT86RF2XX_PARAM_INT         WIRELESS_INT
#define AT86RF2XX_PARAM_SLEEP       WIRELESS_SLP
#define AT86RF2XX_PARAM_RESET       WIRELESS_RST

/**
 * @brief   Initialize board specific hardware, including clock, LEDs and std-IO
 */
void board_init(void);

#ifdef __cplusplus
}
#endif

#endif /* BOARD_H */
/** @} */
