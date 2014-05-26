/*
 * Copyright (C) 2014 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the LGPLv2 License.
 * See the file LICENSE in the top level directory for more details.
 */

/**
 * @ingroup     driver_periph
 * @brief       Low-level UART peripheral driver
 * @{
 *
 * @file        uart.h
 * @brief       Low-level UART peripheral driver interface definitions
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef __PERIPH_UART_H
#define __PERIPH_UART_H

#include <stdint.h>

#include "periph_conf.h"


/**
 * @brief Definition of available UART devices
 *
 * To this point a maximum of 4 UART devices would be available,
 * this should be enough for most applications?!
 */
typedef enum {
#if UART_0_EN
    UART_0 = 0,         /*< UART channel 0 */
#endif
#if UART_1_EN
    UART_1,             /*< UART channel 1 */
#endif
#if UART_2_EN
    UART_2,             /*< UART channel 2 */
#endif
#if UART_3_EN
    UART_3,             /*< UART channel 3 */
#endif
    UART_UNDEFINED      /*< fall-back value */
} uart_t;


/**
 * @brief Initialize a given UART device
 *
 * The UART device will be initialized with the following configuration:
 * - 8 data bits
 * - no parity
 * - 1 stop bit
 * - baud-rate as given
 *
 * @param[in] uart          the UART device to initialize
 * @param[in] baudrate      the desired baud-rate in baud/s
 * @param[in] rx_cb         receive callback is called for every byte the is receive
 *                          in interrupt context
 * @param[in] tx_cb         transmit callback is called when done with sending a byte
 *                          (TX buffer gets empty)
 *
 * @return                  0 on success, -1 for invalid baud-rate, -2 for all other errors
 */
int uart_init(uart_t uart, uint32_t baudrate, void (*rx_cb)(char), void (*tx_cb)(void));

/**
 * @brief Initialize an UART device for (conventional) blocking usage
 *
 * This function initializes the an UART device for usage without interrupts.
 * When initializing with this function, the corresponding read_blocking and
 * write_blocking functions must be used.
 *
 * The blocking mode should only be used for debugging and testing.
 *
 * Same as uart_init(), the UART device is configured with in 8N1 mode with the given baud-rate.
 *
 * @param[in] uart          the UART device to initialize
 * @param[in] baudrate      the desired baud-rate in baud/s
 *
* @return                   0 on success, -1 for invalid baud-rate, -2 for all other errors
 */
int uart_init_blocking(uart_t uart, uint32_t baudrate);

/**
 * @brief Begin a new transmission, on most platforms this function will enable the TX interrupt
 *
 * @param[in] uart          UART device that will start a transmission
 */
void uart_tx_begin(uart_t uart);

/**
 * @brief End a transmission, on most platforms this will disable the TX interrupt
 *
 * @param[in] uart          the UART device that is finished with transmitting stuff
 */
void uart_tx_end(uart_t uart);

/**
 * @brief Write a byte into the UART's send register
 *
 * Writing a byte into while another byte is still waiting to be transferred will override
 * the old byte. This method should be used in the transmit callback routine as in this it
 * is made sure that no old byte is waiting to be transferred.
 *
 * @param[in] uart          the UART device to use for transmission
 * @param[in] data          the byte to write
 *
 * @return                  1 on success, -1 on error
 */
int uart_write(uart_t uart, char data);

/**
 * @brief Read a single character from the given UART device in blocking manner.
 *
 * This function will actively wait until a byte is available in the UART receive
 * register. Consider using the interrupt driven UART mode instead!
 *
 * @param[in] uart          the UART device to read from
 * @param[in] data          the byte to write
 *
 * @return                  1 on success, -1 on error
 */
int uart_read_blocking(uart_t uart, char *data);

/**
 * @brief Write a single byte to the given UART device in blocking manner.
 *
 * Note: in contrast uart_write, this function will actively wait (block) until the UART
 * device is ready to send a new byte. Consider using the interrupt driven UART mode instead.
 *
 * @param[in] uart          the UART device to write to
 * @param[in] data          the byte to send
 *
 * @return                  1 on success, -1 on error
 */
int uart_write_blocking(uart_t uart, char data);


#endif /* __PERIPH_UART_H */
/** @} */
