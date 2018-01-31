/*
 * Copyright (C) 2017 TU Berlin DAI Labor
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_cc1200 cc1200
 * @ingroup     drivers_netdev
 * @brief       TI cc1200
 * @{
 * @file
 * @brief       Public interface for cc1200 driver
 * @author      Anon Mall <anon.mall@gt-arc.com>
 */

#ifndef CC1200_H
#define CC1200_H

#ifdef __cplusplus
extern "C" {
#endif

#include "periph/spi.h"
#include "periph/gpio.h"
#include "cc1200-internal.h"
#include "net/gnrc/nettype.h"
#include "net/ieee802154.h"
#include "net/netdev.h"

/**
 * @brief Struct for holding cc1200 IO parameters
 */
typedef struct cc1200_params {
    spi_t spi;              /**< SPI device */
    spi_clk_t spi_clk;      /**< SPI clock speed to use */
    spi_cs_t cs_pin;        /**< GPIO pin connected to chip select */
    gpio_t int_pin;         /**< GPIO pin connected to the interrupt pin */
    gpio_t reset_pin;       /**< GPIO pin connected to the reset pin */
} cc1200_params_t;

/**
 * @brief forward declaration
 */
typedef struct cc1200 cc1200_t;

/**
 * @brief Struct for holding cc1200 device state
 */
struct cc1200 {
    cc1200_params_t params;                     /**< cc1200 IO configuration */

    cc1200_statistic_t cc1200_statistic;        /**< Statistic values for
                                                     debugging */

    uint8_t radio_state;                        /**< Radio state */
    uint8_t radio_channel;                      /**< current Radio channel */
    uint8_t radio_address;                      /**< current Radio address */
    uint16_t radio_address_short;               /**< current Radio address */
    uint64_t radio_address_long;                /**< current Radio address */
    uint16_t pan_id;                            /**< current pan id */

    cc1200_pkt_buf_t pkt_buf;                   /**< RX/TX buffer */
#ifdef MODULE_GNRC_NETIF
    gnrc_nettype_t proto;                       /**< protocol the radio expects */
#endif
    void (*isr_cb)(void *arg);                  /**< isr callback */
    void *isr_cb_arg;                           /**< isr callback argument */
};

/**
 * @brief Setup cc1200 device parameters
 *
 * @param[in] dev       device struct to set up
 * @param[in] params    struct holding parameters
 *
 * @return always succeeds
 */
int cc1200_setup(cc1200_t *dev, const cc1200_params_t *params);

/**
 * @brief Set cc1200 channel number
 *
 * @param[in] dev       device to work on
 * @param[in] channr    guess what
 *
 * @return nr of set channel on success
 * @return -1 on error
 */
int16_t cc1200_set_channel(cc1200_t *dev, uint8_t channr);


/**
 * @brief Send raw cc1200 packet
 *
 * @param[in] dev       Device to send on
 * @param[in] vector    ptr to iovector to be sent
 * @param[in] count     ptr to iovector to be sent
 *
 * @return size of packet on success
 * @return <0 on error
 */
//int cc1200_send(cc1200_t *dev, cc1200_pkt_t *packet);
int cc1200_send(cc1200_t *dev, const struct iovec *vector, unsigned count);

/**
 * @brief Set cc1200 radio address
 *
 * @param[in] dev   device to query
 *
 * @return nr of currently set address
 */
uint8_t cc1200_get_address(cc1200_t *dev);

/**
 * @brief Set cc1200 radio address
 *
 * @param[in] dev       device to work on
 * @param[in] address   new address
 *
 * @return address set on success
 * @return 0 on error
 */
uint8_t cc1200_set_address(cc1200_t *dev, uint8_t address);

/**
 * @brief Set cc1200 radio address long
 *
 * @param[in] dev       device to work on
 * @param[in] address   new address
 *
 * @return address set on success
 * @return 0 on error
 */
uint64_t cc1200_set_address_long(cc1200_t *dev, uint64_t address);

/**
 * @brief Set cc1200 radio address short
 *
 * @param[in] dev       device to work on
 * @param[in] address   new address
 *
 * @return address set on success
 * @return 0 on error
 */
uint16_t cc1200_set_address_short(cc1200_t *dev, uint16_t address);

/**
 * @brief Set cc1200 pan id
 *
 * @param[in] dev       device to work on
 * @param[in] pan_id    new pan id
 *
 * @return address set on success
 * @return 0 on error
 */
uint16_t cc1200_set_pan_id(cc1200_t *dev, uint16_t pan_id);



/**
 * @brief Set cc1200 monitor mode setting
 *
 * @param[in] dev   device to work on
 * @param[in] mode  mode to set (0 or 1)
 */
void cc1200_set_monitor(cc1200_t *dev, uint8_t mode);

#ifdef __cplusplus
}
#endif

#endif /* CC1200_H */
/** @} */
