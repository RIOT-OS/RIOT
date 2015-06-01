/*
 * Copyright (C) 2015 Martine Lenders <mlenders@inf.fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    net_gnrc_slip SLIP
 * @ingroup     net
 * @brief       Provides a SLIP interface over UART utilizing
 *              @ref driver_periph_uart.
 * @see         <a href="https://www.ietf.org/rfc/rfc1055">RFC 1055</a>
 * @{
 *
 * @file
 * @brief       SLIP interface defintion
 *
 * @author      Martine Lenders <mlenders@inf.fu-berlin.de>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef GNRC_SLIP_H_
#define GNRC_SLIP_H_

#include <inttypes.h>

#include "net/gnrc.h"
#include "periph/uart.h"
#include "ringbuffer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   UART buffer size used for TX and RX buffers
 *
 * Reduce this value if your expected traffic does not include full IPv6 MTU
 * sized packets
 */
#ifndef GNRC_SLIP_BUFSIZE
#define GNRC_SLIP_BUFSIZE       (1500U)
#endif

/**
 * @brief   Device descriptor for SLIP devices
 */
typedef struct {
    uart_t uart;                    /**< the UART interface */
    ringbuffer_t in_buf;            /**< RX buffer */
    ringbuffer_t out_buf;           /**< TX buffer */
    char rx_mem[GNRC_SLIP_BUFSIZE]; /**< memory used by RX buffer */
    char tx_mem[GNRC_SLIP_BUFSIZE]; /**< memory used by TX buffer */
    uint32_t in_bytes;              /**< the number of bytes received of a
                                     *   currently incoming packet */
    uint16_t in_esc;                /**< receiver is in escape mode */
    kernel_pid_t slip_pid;          /**< PID of the device thread */
} gnrc_slip_dev_t;

/**
 * @brief   auto_init struct holding SLIP initalization params
 */
typedef struct xbee_params {
    uart_t uart;            /**< UART interfaced the device is connected to */
    uint32_t baudrate;      /**< baudrate to use */
} gnrc_slip_params_t;

/**
 * @brief   Initializes a new @ref net_gnrc_slip control thread for UART device
 *          @p uart
 *
 * @param[in] dev           un-initialized SLIP device descriptor
 * @param[in] uart          UART device to use
 * @param[in] baudrate      baudrate to use
 * @param[in] stack         stack memory to use for the SLIP devices thread
 * @param[in] stack_size    size of @p stack
 * @param[in] priority      priority for the newly created thread
 *
 * @return  PID of SLIP thread on success
 * @return  -EFAULT, if slip thread could not be created
 * @return  -ENODEV, if gnrc_slip_dev_t::uart of @p dev was no valid UART
 */
kernel_pid_t gnrc_slip_init(gnrc_slip_dev_t *dev, uart_t uart, uint32_t baudrate,
                            char *stack, size_t stack_size, char priority);

#ifdef __cplusplus
}
#endif

#endif /* __SLIP_H_ */
/** @} */
