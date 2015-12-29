/*
 * Copyright (C) 2014 Freie Universität Berlin
 *               2015 Kaspar Schleiser <kaspar@schleiser.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_cc112x CC110x
 * @ingroup     drivers_netdev
 * @brief       TI CC110x
 * @{
 * @file
 * @brief       Public interface for cc112x driver
 * @author      Kaspar Schleiser <kaspar@schleiser.de>
 */

#ifndef CC110X_H
#define CC110X_H

#ifdef __cplusplus
extern "C" {
#endif

#include "periph/spi.h"
#include "periph/gpio.h"
#include "../cc112x/include/cc112x-internal.h"

/**
 * @brief Struct for holding cc112x IO parameters
 */
typedef struct cc112x_params {
    spi_t spi;          /**< what */
    gpio_t cs;          /**< does */
    gpio_t gpio0;        /**< this */
    gpio_t gpio1;
    gpio_t gpio2;        /**< look */
    gpio_t gpio3;        /**< like */
    gpio_t reset;
} cc112x_params_t;

/**
 * @brief forward declaration
 */
typedef struct cc112x cc112x_t;

/**
 * @brief Struct for holding cc112x device state
 */
struct cc112x {
    cc112x_params_t params;                     /**< cc112x IO configuration */

    cc112x_statistic_t cc112x_statistic;        /**< Statistic values for
                                                     debugging */

    uint8_t radio_state;                        /**< Radio state */
    uint8_t radio_channel;                      /**< current Radio channel */
    uint8_t radio_address;                      /**< current Radio address */

    cc112x_pkt_buf_t pkt_buf;                   /**< RX/TX buffer */
    void (*isr_cb)(cc112x_t *dev, void* arg);   /**< isr callback */
    void *isr_cb_arg;                           /**< isr callback argument */
};

bool cc112x_test(cc112x_t *dev);

/**
 * @brief Setup cc112x device parameters
 *
 * @param[in] dev       device struct to set up
 * @param[in] params    struct holding parameters
 *
 * @return always succeeds
 */
int cc112x_setup(cc112x_t *dev, const cc112x_params_t *params);

/**
 * @brief Set cc112x channel number
 *
 * @param[in] dev       device to work on
 * @param[in] channr    guess what
 *
 * @return nr of set channel on success
 * @return -1 on error
 */
int16_t cc112x_set_channel(cc112x_t *dev, uint8_t channr);

/**
 * @brief	Set cc112x output power
 * @note	Set the closest possible output power.
 *
 * @param[in] dev		device to work on
 * @param[in] power		power in dBm
 *
 * @return
 */

/**
 * @brief Send raw cc112x packet
 *
 * @param[in] dev       Device to send on
 * @param[in] packet    ptr to packet to be sent
 *
 * @return size of packet on success
 * @return <0 on error
 */
int cc112x_send(cc112x_t *dev, cc112x_pkt_t *packet);

/**
 * @brief Set cc112x radio address
 *
 * @param[in] dev   device to query
 *
 * @return nr of currently set address
 */
uint8_t cc112x_get_address(cc112x_t *dev);

/**
 * @brief Set cc112x radio address
 *
 * @param[in] dev       device to work on
 * @param[in] address   new address
 *
 * @return address set on success
 * @return 0 on error
 */
uint8_t cc112x_set_address(cc112x_t *dev, uint8_t address);


/**
 * @brief Set cc112x monitor mode setting
 *
 * @param[in] dev   device to work on
 * @param[in] mode  mode to set (0 or 1)
 */
void cc112x_set_monitor(cc112x_t *dev, uint8_t mode);

#ifdef __cplusplus
}
#endif

#endif /* CC110X_H */
/** @} */
