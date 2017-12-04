/*
 * Copyright (C) 2017 Robert Hartung <hartung@ibr.cs.tu-bs.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     boards_inga_common
 * @{
 *
 * @file
 * @brief       Common board definitions for the INGA boards
 *
 * @author      Robert Hartung <hartung@ibr.cs.tu-bs.de>
 *
 * @}
 */

#ifndef INGA_COMMON_BOARD_COMMON_H_
#define INGA_COMMON_BOARD_COMMON_H_

#ifdef __cplusplus
extern "C" {
#endif

#ifndef UART_STDIO_BAUDRATE
#define UART_STDIO_BAUDRATE               (19200)
#endif

/**
 * Context swap defines
 * Setup to use PA0 which is pin change interrupt 0 (PCINT0)
 * This emulates a software triggered interrupt
 */
#define AVR_CONTEXT_SWAP_INIT do { \
            DDRA |= (1 << DDA0); \
            PCICR |= (1 << PCIE0); \
            PCMSK0 |= (1 << PCINT0); \
} while (0)
#define AVR_CONTEXT_SWAP_INTERRUPT_VECT         PCINT0_vect
#define AVR_CONTEXT_SWAP_INTERRUPT_VECT_NUM     PCINT0_vect_num
#define AVR_CONTEXT_SWAP_TRIGGER                PORTA ^= (1 << PA0)

/**
 * @name    at86rf233 configuration
 * @{
 */
#define AT86RF2XX_PARAMS_BOARD {.spi = SPI_DEV(0), \
                                .spi_clk = SPI_CLK_5MHZ, \
                                .cs_pin = GPIO_PIN(PORT_B, 4),\
                                .int_pin = GPIO_PIN(PORT_D, 6),\
                                .sleep_pin = GPIO_PIN(PORT_B, 3),\
                                .reset_pin = GPIO_PIN(PORT_B, 1)}
/** @} */

/**
 * @name    xtimer configuration values
 * @{
 */
#define XTIMER_WIDTH                      (16)
#define XTIMER_BACKOFF                    (40)
#define XTIMER_DEV                        (0)
#define XTIMER_CHAN                       (0)
/** @} */

#ifdef __cplusplus
}
#endif

/** @} */
#endif /*  INGA_COMMON_BOARD_COMMON_H_ */
