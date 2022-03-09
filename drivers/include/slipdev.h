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
#include "tsrb.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup drivers_slipdev_config     SLIP Network driver compile configuration
 * @ingroup config_drivers_netdev
 * @{
 */
/**
 * @brief   UART buffer size used for TX and RX buffers
 *
 * Reduce this value if your expected traffic does not include full IPv6 MTU
 * sized packets.
 *
 * @pre Needs to be power of two and `<= INT_MAX`
 */
#ifdef CONFIG_SLIPDEV_BUFSIZE_EXP
#define CONFIG_SLIPDEV_BUFSIZE (1<<CONFIG_SLIPDEV_BUFSIZE_EXP)
#endif

#ifndef CONFIG_SLIPDEV_BUFSIZE
#define CONFIG_SLIPDEV_BUFSIZE (2048U)
#endif
/** @} */

/**
 * @name    Device state definitions
 * @anchor  drivers_slipdev_states
 * @{
 */
enum {
    /**
     * @brief   Device is in no mode (currently did not receiving any data frame)
     */
    SLIPDEV_STATE_NONE = 0,
    /**
     * @brief   Device writes handles data as network device
     */
    SLIPDEV_STATE_NET,
    /**
     * @brief   Device writes received data to stdin
     */
    SLIPDEV_STATE_STDIN,
};
/** @} */

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
    tsrb_t inbuf;                           /**< RX buffer */
    uint8_t rxmem[CONFIG_SLIPDEV_BUFSIZE];  /**< memory used by RX buffer */
    /**
     * @brief   Device state
     * @see     [Device state definitions](@ref drivers_slipdev_states)
     */
    uint8_t state;
} slipdev_t;

/**
 * @brief   Setup a slipdev device state
 *
 * @param[in] dev       device descriptor
 * @param[in] params    parameters for device initialization
 * @param[in] index     index of @p params in a global parameter struct array.
 *                      If initialized manually, pass a unique identifier instead.
 */
void slipdev_setup(slipdev_t *dev, const slipdev_params_t *params, uint8_t index);

#ifdef __cplusplus
}
#endif

#endif /* SLIPDEV_H */
/** @} */
