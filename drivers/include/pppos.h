/*
 * Copyright (C) 2018 Max van Kessel
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_pppos Point to Point Protocol over Serial network device
 * @ingroup     drivers_netdev
 * @brief       PPPoS network device over @ref drivers_periph_uart
 * @note        PPPoS uses HDLC like framing
 * @{
 *
 * @file
 * @brief   PPPoS device definitions
 *
 * @author  Max van Kessel
 */
#ifndef PPPOS_H
#define PPPOS_H

#include <stdint.h>
#include <unistd.h>

#include "net/netdev/ppp.h"
#include "periph/uart.h"
#include "periph/gpio.h"
#include "tsrb.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   UART buffer size used for RX buffers
 *
 * Not planning to store data here very long.
 *
 * @pre Needs to be power of two and `<= INT_MAX`
 */
#ifndef PPPOS_BUFSIZE
#define PPPOS_BUFSIZE (128U)
#endif

/**
 * @brief   Configuration parameters for PPP over Serial
 */
typedef struct {
    uart_t uart;        /**< UART interface the device is connected to */
    uint32_t baudrate;  /**< baudrate to use with pppos_params_t::uart */
} pppos_params_t;

/**
 * @brief   Device descriptor for PPP over Serial
 *
 * @extends netdev_ppp_t
 */
typedef struct {
    netdev_ppp_t netdev;            /**< parent class */
    pppos_params_t config;          /**< configuration parameters */
    struct {
        uint32_t rx;
        uint32_t tx;
    } accm;                         /**< async character control map */

    tsrb_t inbuf;                   /**< ring buffer definition */
    uint8_t rxmem[PPPOS_BUFSIZE];   /**< memory used by RX buffer */
} pppos_t;

/**
 * @brief   Setup a PPP over serial device
 *
 * @param[in] dev       device descriptor
 * @param[in] params    parameters for device initialization
 */
void pppos_setup(pppos_t *dev, const pppos_params_t *params);

/**
 * @brief   New byte entry
 * @note    Should be called if no uart is set.
 *
 * @param[in] arg   Should be of type pppos_t
 * @param[in] byte  New byte
 */
void pppos_rx_write_one(void *arg, uint8_t byte);

#ifdef __cplusplus
}
#endif

#endif /* PPPOS_H */
/** @} */
