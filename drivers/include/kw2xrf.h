/*
 * Copyright (C) 2015 Phytec Messtechnik GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_kw2xrf kw2x radio-driver
 * @ingroup     drivers
 * @brief       Device driver for the Freescale KW2xD radio
 * @{
 *
 * @file
 * @brief       Interface definition for the KW2xD device driver
 *
 * @author      Johann Fischer <j.fischer@phytec.de>
 * @author      Jonas Remmert <j.remmert@phytec.de>
 */

#ifndef MKW2XDRF_H_
#define MKW2XDRF_H_

#include <stdint.h>

#include "board.h"
#include "periph/spi.h"
#include "periph/gpio.h"
#include "net/gnrc/netdev.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Maximum packet length
 */
#define KW2XRF_MAX_PKT_LENGTH         (127U)

/**
 * @brief   Default protocol for data that is coming in
 */
#ifdef MODULE_GNRC_SIXLOWPAN
#define KW2XRF_DEFAULT_PROTOCOL       GNRC_NETTYPE_SIXLOWPAN
#else
#define KW2XRF_DEFAULT_PROTOCOL       GNRC_NETTYPE_UNDEF
#endif

/**
 * @brief   Default short address used after initialization
 */
#define KW2XRF_DEFAULT_SHORT_ADDR     (0x0002)

/**
 * @brief   Default short address used after initialization
 */
#define KW2XRF_DEFAULT_ADDR_LONG  (0x0000000000000000)

/**
 * @brief   Default PAN ID used after initialization
 */
#define KW2XRF_DEFAULT_PANID          (0x0023)

/**
 * @brief   Default channel used after initialization
 */
#define KW2XRF_DEFAULT_CHANNEL        (26U)

/**
 * @brief   Default TX_POWER in dbm used after initialization
 */
#define KW2XRF_DEFAULT_TX_POWER       (0)

/**
 * @brief   Maximum output power of the kw2x device in dBm
 */
#define MKW2XDRF_OUTPUT_POWER_MAX      (8)

/**
 * @brief   Minimum output power of the kw2x device in dBm
 */
#define MKW2XDRF_OUTPUT_POWER_MIN      (-35)

/**
 * @brief   Internal device option flags
 * @{
 */
#define KW2XRF_OPT_AUTOACK       (0x0001)  /**< auto ACKs active */
#define KW2XRF_OPT_CSMA          (0x0002)  /**< CSMA active */
#define KW2XRF_OPT_PROMISCUOUS   (0x0004)  /**< promiscuous mode active */
#define KW2XRF_OPT_PRELOADING    (0x0008)  /**< preloading enabled */
#define KW2XRF_OPT_TELL_TX_START (0x0010)  /**< notify MAC layer on TX start */
#define KW2XRF_OPT_TELL_TX_END   (0x0020)  /**< notify MAC layer on TX finished */
#define KW2XRF_OPT_TELL_RX_START (0x0040)  /**< notify MAC layer on RX start */
#define KW2XRF_OPT_TELL_RX_END   (0x0080)  /**< notify MAC layer on RX finished */
#define KW2XRF_OPT_RAWDUMP       (0x0100)  /**< pass RAW frame data to upper layer */
#define KW2XRF_OPT_SRC_ADDR_LONG (0x0200)  /**< send data using long source address */
#define KW2XRF_OPT_USE_SRC_PAN   (0x0400)  /**< do not compress source PAN ID */
/** @} */

/**
 * @brief   kw2xrf device descriptor
 */
typedef struct {
    /* netdev fields */
    gnrc_netdev_driver_t const *driver;   /**< Pointer to the devices interface */
    gnrc_netdev_event_cb_t event_cb;      /**< Netdev event callback */
    kernel_pid_t mac_pid;                 /**< The driver's thread's PID */
    /* driver specific fields */
    uint8_t buf[KW2XRF_MAX_PKT_LENGTH];   /**< Buffer for incoming or outgoing packets */
    netopt_state_t state;                 /**< Variable to keep radio driver's state */
    uint8_t seq_nr;                       /**< Next packets sequence number */
    uint16_t radio_pan;                   /**< The PAN the radio device is using */
    uint8_t radio_channel;                /**< The channel the radio device is using */
    uint8_t addr_short[2];                /**< The short address the radio device is using */
    uint8_t addr_long[8];                 /**< The long address the radio device is using */
    uint16_t option;                      /**< Bit field to save enable/disable options */
    int8_t tx_power;                      /**< The current tx-power setting of the device */
    gnrc_nettype_t proto;                 /**< Protocol the interface speaks */
} kw2xrf_t;

/**
 * @brief   Initialize the given KW2XRF device
 * @param[out] dev          device descriptor
 * @param[in] spi           SPI bus the device is connected to
 * @param[in] spi_speed     SPI speed to use
 * @param[in] cs_pin        GPIO pin connected to chip select
 * @param[in] int_pin       GPIO pin connected to the interrupt pin
 *
 * @return                  0 on success
 * @return                  <0 on error
 */
int kw2xrf_init(kw2xrf_t *dev, spi_t spi, spi_speed_t spi_speed,
                    gpio_t cs_pin, gpio_t int_pin);

/**
 * @brief struct holding all params needed for device initialization
 */
typedef struct kw2xrf_params {
    spi_t spi;              /**< SPI bus the device is connected to */
    spi_speed_t spi_speed;  /**< SPI speed to use */
    gpio_t cs_pin;          /**< GPIO pin connected to chip select */
    gpio_t int_pin;         /**< GPIO pin connected to the interrupt pin */
} kw2xrf_params_t;

/**
 * @brief Reference to the KW2XRF driver interface
 */
extern const gnrc_netdev_driver_t kw2xrf_driver;

#ifdef __cplusplus
}
#endif

#endif
/** @} */
