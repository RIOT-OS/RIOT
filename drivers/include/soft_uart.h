/*
 * Copyright (C) 2020 ML!PA Consulting GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

#pragma once

/**
 * @defgroup    drivers_soft_uart Software UART
 * @ingroup     drivers_soft_periph
 * @brief       Software implemented UART
 *
 * This module provides a software implemented Universal Asynchronous Receiver Transmitter.
 * It is intended to be used in situation where hardware UART is not available.
 * The signatures of the functions are similar to the functions declared in uart.h
 *
 * Currently sending and receiving is not possible at the same time, so loopback operation
 * is not possible.
 *
 * @{
 *
 * @file
 * @brief       Software UART port descriptor definition
 *
 * @author      Benjamin Valentin <benjamin.valentin@ml-pa.com>
 */

#include "periph/gpio.h"
#include "periph/uart.h"
#include "periph/timer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief invert the level of the TX signal
 */
#define SOFT_UART_FLAG_INVERT_TX    0x1

/**
 * @brief invert the level of the RX signal
 */
#define SOFT_UART_FLAG_INVERT_RX    0x2

/**
 * @brief Software UART port descriptor
 */
typedef struct {
    gpio_t rx_pin;          /**< RX pin */
    gpio_t tx_pin;          /**< TX pin */
    tim_t rx_timer;         /**< Hardware timer used for RX */
    tim_t tx_timer;         /**< Hardware timer used for TX */
    uint32_t timer_freq;    /**< Operating frequency of the timer.
                                 Should be a multiple of baudrate */
    uint8_t flags;          /**< Soft UART flags */
} soft_uart_conf_t;

/**
 * @brief Software UART type definition
 */
typedef unsigned soft_uart_t;

/**
 * @brief   Initialize a given UART device
 *
 * The UART device will be initialized with the following configuration:
 * - 8 data bits
 * - no parity
 * - 1 stop bit
 * - baudrate as given
 *
 * If no callback parameter is given (rx_cb := NULL), the UART will be
 * initialized in TX only mode.
 *
 * @param[in] uart          UART device to initialize
 * @param[in] baudrate      desired symbol rate in baud
 * @param[in] rx_cb         receive callback, executed in interrupt context once
 *                          for every byte that is received (RX buffer filled),
 *                          set to NULL for TX only mode
 * @param[in] arg           optional context passed to the callback functions
 *
 * @return                  UART_OK on success
 * @return                  UART_NODEV on invalid UART device
 * @return                  UART_NOBAUD on inapplicable baudrate
 * @return                  UART_INTERR on other errors
 */
int soft_uart_init(soft_uart_t uart, uint32_t baudrate, uart_rx_cb_t rx_cb, void *arg);

/**
 * @brief   Setup parity, data and stop bits for a given UART device
 *
 * @param[in] uart          UART device to configure
 * @param[in] data_bits     number of data bits in a UART frame
 * @param[in] parity        parity mode
 * @param[in] stop_bits     number of stop bits in a UART frame
 *
 * @return                  UART_OK on success
 * @return                  UART_NOMODE on other errors
 */
int soft_uart_mode(soft_uart_t uart, uart_data_bits_t data_bits, uart_parity_t parity,
                   uart_stop_bits_t stop_bits);

/**
 * @brief   Write data from the given buffer to the specified UART device
 *
 * This function is blocking, as it will only return after @p len bytes from the
 * given buffer have been send. The way this data is send is up to the
 * implementation: active waiting, interrupt driven, DMA, etc.
 *
 * @param[in] uart          UART device to use for transmission
 * @param[in] data          data buffer to send
 * @param[in] len           number of bytes to send
 *
 */
void soft_uart_write(soft_uart_t uart, const uint8_t *data, size_t len);

/**
 * @brief   Power on the given UART device
 *
 * @param[in] uart          the UART device to power on
 */
void soft_uart_poweron(soft_uart_t uart);

/**
 * @brief Power off the given UART device
 *
 * @param[in] uart          the UART device to power off
 */
void soft_uart_poweroff(soft_uart_t uart);

#ifdef __cplusplus
}
#endif

/** @} */
