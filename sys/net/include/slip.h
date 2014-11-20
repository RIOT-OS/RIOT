/*
 * Copyright (C) 2014 Martin Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    slip SLIP transceiver
 * @ingroup     net
 * @{
 *
 * @file        slip.h
 * @brief       Provides a SLIP interface over UART.
 * @see         <a href="https://www.ietf.org/rfc/rfc1055">RFC 1055</a>
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * @}
 */

#ifndef __SLIP_H_
#define __SLIP_H_

#include <stdint.h>

#include "netapi.h"
#include "periph/uart.h"
#include "ringbuffer.h"

#ifdef __cplusplus
extern "C" {
#endif

#if !UART_NUMOF
/**
 * @brief   Guard type in the case that the board does not implement the new
 *          driver model
 */
typedef uint8_t uart_t;
#endif

/**
 * Initializes a new @ref slip control thread for UART device *uart*. If
 * the board does not support the uart_t type it falls back to using
 * board_uart0.h
 *
 * @param[in] uart      The (uninitialized) UART device, ignored if UART_NUMOF == 0 or not defined
 * @param[in] baudrate  Symbole rate for the UART device
 * @param[in] in_buf    Ringbuffer to store the incoming data from the UART in.
 *                      Must hold at least L3  packet.
 *
 * @return The PID of the @ref slip control thread
 */
kernel_pid_t slip_init(uart_t uart, uint32_t baudrate, ringbuffer_t *in_buf);

/**
 * @func    slip_send_l3_packet
 * @brief   Send a layer 3 packet over the UART device
 *
 * @param[in] pid               The PID for the @ref slip control thread
 * @param[in] upper_layer_hdrs  All upper layer headers including layer 3.
 * @param[in] data              Data (*without* all upper layer headers) to send
 *                              over the UART device
 * @param[in] data_len          Length of *data*
 */

#ifdef MODULE_NETAPI
static inline int slip_send_l3_packet(kernel_pid_t pid, netdev_hlist_t *upper_layer_hdrs,
                                      void *data, size_t data_len)
{
    return netapi_send_data(pid, upper_layer_hdrs, NULL, 0, data, data_len);
}
#else
int slip_send_l3_packet(kernel_pid_t pid, netdev_hlist_t *upper_layer_hdrs,
                        void *data, size_t data_len);
#endif

#ifdef __cplusplus
}
#endif

#endif /* __SLIP_H_ */
