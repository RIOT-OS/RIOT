/*
 * Copyright (C) 2014-2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    driver_periph_uart UART
 * @ingroup     driver_periph
 * @brief       Low-level UART peripheral driver
 * @{
 *
 * @file
 * @brief       Low-level UART peripheral driver interface definition
 *
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef PERIPH_UART_H
#define PERIPH_UART_H

#include <stdint.h>

#include "periph_cpu.h"
#include "periph_conf.h"
/* TODO: remove once all platforms are ported to this interface */
#include "periph/dev_enums.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Make sure the number of available UART devices is defined
 * @{
 */
#ifndef UART_NUMOF
#error "UART_NUMOF undefined for the target platform"
#endif
/** @} */

/**
 * @brief   Define default UART type identifier
 * @{
 */
#ifndef HAVE_UART_T
typedef int uart_t;
#endif
/** @} */

/**
 * @brief   Default UART undefined value
 * @{
 */
#ifndef UART_UNDEF
#define UART_UNDEF      (-1)
#endif
/** @} */

/**
 * @brief   Default UART device access macro
 * @{
 */
#ifndef UART_DEV
#define UART_DEV(x)     (x)
#endif
/** @} */

/**
 * @brief   Signature for receive interrupt callback
 *
 * @param[in] arg           context to the callback (optional)
 * @param[in] data          the byte that was received
 */
typedef void(*uart_rx_cb_t)(void *arg, char data);

/**
 * @brief   Signature for the transmit complete interrupt callback
 *
 * @param[in] arg           context to the callback (optional)
 *
 * @return                  1 if more data is to be send
 * @return                  0 if no more data is to be send
 */
typedef int(*uart_tx_cb_t)(void *arg);

/**
 * @brief   Interrupt context for a UART device
 * @{
 */
#ifndef HAVE_UART_ISR_CTX_T
typedef struct {
    uart_rx_cb_t rx_cb;     /**< data received interrupt callback */
    uart_tx_cb_t tx_cb;     /**< data transmission done interrupt callback */
    void *arg;              /**< argument to both callback routines */
} uart_isr_ctx_t;
#endif
/** @} */

/**
 * @brief   Initialize a given UART device
 *
 * The UART device will be initialized with the following configuration:
 * - 8 data bits
 * - no parity
 * - 1 stop bit
 * - baudrate as given
 *
 * @param[in] uart          UART device to initialize
 * @param[in] baudrate      desired baudrate in baud/s
 * @param[in] rx_cb         receive callback, executed in interrupt context once
 *                          for every byte that is received (RX buffer filled)
 * @param[in] tx_cb         transmit callback, executed in interrupt context
 *                          for each finished byte transfer (TX buffer empty)
 * @param[in] arg           optional context passed to the callback functions
 *
 * @return                  0 on success
 * @return                  -1 on invalid UART device
 * @return                  -2 on inapplicable baudrate
 * @return                  -3 on other errors
 */
int uart_init(uart_t uart, uint32_t baudrate,
              uart_rx_cb_t rx_cb, uart_tx_cb_t tx_cb, void *arg);

/**
 * @brief   Begin a new transmission
 *
 * When sending buffered data, we need to tell the UART device, whenever there
 * is new data waiting to be transferred. In that case, this function should be
 * called. On most platforms this function will simply enable the TX interrupt
 * leading to a call of the TX callback function.
 *
 * @param[in] uart          UART device that will start a transmission
 */
void uart_tx_begin(uart_t uart);

/**
 * @brief   Write a byte into the UART's send register
 *
 * Calling this function while another byte is still waiting to be transferred,
 * this call will overwrite the waiting byte. This method should be used in the
 * TX callback routine, as here it is made sure that no old byte is waiting to
 * be transferred.
 *
 * @param[in] uart          UART device to use for transmission
 * @param[in] data          data byte to send
 */
void uart_write(uart_t uart, char data);

/**
 * @brief   Write a single byte to the given UART device (blocking)
 *
 * In contrast to *uart_write*, this function will block (active waiting) until
 * the UART device is ready to send the new byte.
 *
 * @note Use for testing purposes only!
 *
 * @param[in] uart          UART device to write to
 * @param[in] data          data byte to send
 */
void uart_write_blocking(uart_t uart, char data);

/**
 * @brief   Power on the given UART device
 *
 * @param[in] uart          the UART device to power on
 */
void uart_poweron(uart_t uart);

/**
 * @brief Power off the given UART device
 *
 * @param[in] uart          the UART device to power off
 */
void uart_poweroff(uart_t uart);

#ifdef __cplusplus
}
#endif

#endif /* PERIPH_UART_H */
/** @} */
