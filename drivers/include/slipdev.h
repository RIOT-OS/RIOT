/*
 * Copyright (C) 2015-17 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_slipdev SLIP network device
 * @ingroup     drivers_netdev
 * @brief       SLIP network device over @ref drivers_periph_uart
 * @see         [RFC 1055](https://github.com/RIOT-OS/RIOT/pull/6487)
 * @{
 *
 * @file
 * @brief   SLIP device definitions
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */
#ifndef SLIPDEV_H
#define SLIPDEV_H

#include <stdint.h>

#include "cib.h"
#include "net/netdev.h"
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
#ifndef SLIPDEV_BUFSIZE
#define SLIPDEV_BUFSIZE (1500U)
#endif

/**
 * @brief   Packet FIFO size
 *
 * @note    For GNRC it is recommended to have it the same size as the link-layer
 *          thread's message queue, but it MUST be of power of 2
 */
#ifndef SLIPDEV_PKTFIFO_SIZE
#define SLIPDEV_PKTFIFO_SIZE    (8U)
#endif

/**
 * @brief   Configuration parameters for a slipdev
 */
typedef struct {
    uart_t uart;        /**< UART interface the device is connected to */
    uint32_t baudrate;  /**< baudrate to use with slipdev_params_t::uart */
} slipdev_params_t;

/**
 * @brief   Device descriptor for slipdev
 *
 * @extends netdev_t
 */
typedef struct {
    netdev_t netdev;                        /**< parent class */
    slipdev_params_t config;                /**< configuration parameters */
    ringbuffer_t inbuf;                     /**< RX buffer */
    char rxmem[SLIPDEV_BUFSIZE];            /**< memory used by RX buffer */
    uint16_t pktfifo[SLIPDEV_PKTFIFO_SIZE]; /**< FIFO of sizes of fully received
                                             *   packets */
    cib_t pktfifo_idx;                      /**< CIB for slipdev_t::pktfifo */
    uint16_t inbytes;                       /**< the number of bytes received of
                                             *   a currently incoming packet */
    uint16_t inesc;                         /**< device previously received an escape
                                             *   byte */
} slipdev_t;

/**
 * @brief   Setup a slipdev device state
 *
 * @param[in] dev       device descriptor
 * @param[in] params    parameters for device initialization
 */
void slipdev_setup(slipdev_t *dev, const slipdev_params_t *params);

#ifdef __cplusplus
}
#endif

#endif /* SLIPDEV_H */
/** @} */
