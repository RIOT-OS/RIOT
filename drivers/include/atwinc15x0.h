/*
 * Copyright (C) 2020 Gunar Schorcht
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_netdev
 * @brief       Netdev Driver for the Microchip ATWINC15x0 WiFi Module
 * @{
 *
 * @file
 * @brief       Public interface for ATWINC15x0 netdev driver
 *
 * @author      Gunar Schorcht <gunar@schorcht.net>
 */

#ifndef ATWINC15X0_H
#define ATWINC15X0_H

#include "bsp/include/nm_bsp.h"
#include "net/ethernet.h"
#include "net/wifi.h"
#include "net/netdev.h"
#include "periph/gpio.h"
#include "periph/spi.h"
#include "ringbuffer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Enable reception of broadcast frames
 *
 *          If your application does not rely on receiving broadcast messages
 *          you can disable this to save power.
 */
#ifndef CONFIG_ATWINC15X0_RECV_BCAST
#define CONFIG_ATWINC15X0_RECV_BCAST (1)
#endif

/**
 * @brief   Maximum number of supported entries in a scan result of
 *          an ATWINC15x0 transceiver
 */
#ifndef CONFIG_ATWINC15X0_SCAN_LIST_NUMOF
#define CONFIG_ATWINC15X0_SCAN_LIST_NUMOF   (3)
#endif

/**
 * @brief   ATWINC15x0 hardware and global parameters
 */
typedef struct {
    spi_t spi;              /**< SPI device */
    spi_clk_t spi_clk;      /**< SPI clock speed used */
    gpio_t ssn_pin;         /**< SPI SS pin (slave select LOW active) */
    gpio_t reset_pin;       /**< RESET_N pin (LOW active) */
    gpio_t irq_pin;         /**< IRQN pin (LOW active) */
    gpio_t chip_en_pin;     /**< CHIP_EN pin */
    gpio_t wake_pin;        /**< WAKE pin */
} atwinc15x0_params_t;

/**
 * @brief   ATWINC15x0 internal states
 */
typedef enum {
    ATWINC15X0_STATE_SLEEP,                     /**< Sleep state */
    ATWINC15X0_STATE_DISCONNECTING,             /**< Disconnect received when connected before */
    ATWINC15X0_STATE_DISCONNECTED,              /**< Disconnect state */
    ATWINC15X0_STATE_DISCONNECTED_SCANNING,     /**< Scanning state when disconnected */
    ATWINC15X0_STATE_DISCONNECTED_CONNECTING,   /**< Connecting state where disconnected before */
    ATWINC15X0_STATE_CONNECTED,                 /**< Connected state */
    ATWINC15X0_STATE_CONNECTED_SCANNING,        /**< Scanning state when connected */
    ATWINC15X0_STATE_CONNECTED_CONNECTING,      /**< Connecting state where disconnect
                                                     event is not yet received */
} atwinc15x0_state_t;

/**
 * @brief   ATWINC15x0 device descriptor type
 */
typedef struct atwinc15x0 {
    netdev_t netdev;            /**< Pulls in the netdev fields */
    atwinc15x0_params_t params; /**< Device initialization parameters */
    atwinc15x0_state_t state;   /**< Device state */
    char ap[ETHERNET_ADDR_LEN]; /**< BSSID of current AP */
#if IS_USED(MODULE_ATWINC15X0_DYNAMIC_CONNECT) || defined(DOXYGEN)
    char ssid[WIFI_SSID_LEN_MAX + 1]; /**< SSID of current AP */
#endif
    uint8_t channel;            /**< Channel used for current AP */
    int8_t rssi;                /**< RSSI last measured by the WiFi module */

    uint8_t* rx_buf;            /**< Incoming packet in receive buffer */
    uint16_t rx_len;            /**< Length of an incoming packet, if there
                                     is no packet in the buffer, it is 0 */

    tpfNmBspIsr bsp_isr;        /**< Board support package ISR  */
    bool bsp_irq_enabled;       /**< Board support package interrupt enabled */
} atwinc15x0_t;

/**
 * @brief   Setup the ATWINC15x0 WiFi module
 *
 * @param[in] dev     Device descriptor
 * @param[in] params  Parameters for device initialization
 * @param[in] idx     Index in the params struct
 */
void atwinc15x0_setup(atwinc15x0_t *dev, const atwinc15x0_params_t *params, uint8_t idx);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* ATWINC15X0_H */
