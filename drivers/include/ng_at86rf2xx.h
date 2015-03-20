/*
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     drivers_ng_at86rf2xx
 * @{
 *
 * @file
 * @brief       t.b.d
 *
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 */

#ifndef NG_AT86RF2XX_H_
#define NG_AT86RF2XX_H_

#include <stdint.h>

#include "board.h"
#include "periph/spi.h"
#include "periph/gpio.h"

#include "net/ng_netdev.h"

#include "ng_at86rf2xx.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief at86rf231's minimum TX power in dBm
 */
#define NG_AT86RF2XX_TX_POWER_MIN  -17

/**
 * @brief at86rf231's maximum TX power in dBm
 */
#define NG_AT86RF2XX_TX_POWER_MAX  4

/**
  * @brief at86rf231's lowest supported channel
  */
#define NG_AT86RF2XX_MIN_CHANNEL       (11)

/**
  * @brief at86rf231's highest supported channel
  */
#define NG_AT86RF2XX_MAX_CHANNEL       (26)

#define NG_AT86RF2XX_MAX_PKT_LENGTH    (127)

extern const ng_netdev_driver_t ng_at86rf2xx_driver;

void ng_at86rf2xx_irq_handler(void *arg);



/**
 * @brief Device descriptor for AT86RF2XX radio devices
 * @details First three fields are for compliance to ng_netdev_t
 */
typedef struct {
    const ng_netdev_driver_t *driver;     /**< pointer to the devices interface */
    ng_netdev_event_cb_t event_cb;  /**< netdev event callback */
    kernel_pid_t mac_pid;           /**< the driver's thread's PID */
    spi_t spi;                      /**< SPI device the radio device is connected to */
    spi_speed_t spi_speed;          /**< SPI bus speed the radio device is connected to */
    gpio_t cs_pin;                  /**< Radio device's chip select pin */
    gpio_t sleep_pin;               /**< Radio device's sleep pin */
    gpio_t reset_pin;               /**< Radio device's reset pin */
    gpio_t int_pin;                 /**< Radio device's interrupt pin */
    gpio_cb_t irq_handler;          /**< Interrupt handler */
    ng_netconf_state_t state;       /**< Variable to keep radio driver's state */
    uint8_t seq_nr;                 /**< Next packets sequence number */
    uint16_t radio_pan;             /**< The PAN the radio device is using */
    uint8_t radio_channel;          /**< The channel the radio device is using */
    uint16_t radio_address;         /**< The short address the radio device is using */
    uint64_t radio_address_long;    /**< The long address the radio device is using */
    uint16_t options;               /**< Bit field to save enable/disable options */
} ng_at86rf2xx_t;


void ng_at86rf2xx_initialise(ng_at86rf2xx_t *dev);

int ng_at86rf2xx_change_state(ng_at86rf2xx_t *dev, ng_netconf_state_t state);

int ng_at86rf2xx_send(ng_netdev_t *device, ng_pktsnip_t *pkt);

int ng_at86rf2xx_get(ng_netdev_t *device, ng_netconf_opt_t opt,
                     void *value, size_t value_len);

int ng_at86rf2xx_set(ng_netdev_t *device, ng_netconf_opt_t opt,
                     void *value, size_t value_len);

#ifdef __cplusplus
}
#endif

#endif /* NG_AT86RF2XX_H_ */
/** @} */
