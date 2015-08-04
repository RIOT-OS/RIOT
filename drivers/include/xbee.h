/*
 * Copyright (C) 2014 INRIA
 * Copyright (C) 2015 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_xbee XBee driver
 * @ingroup     drivers
 * @brief       High-level driver for the XBee S1 802.15.4 modem
 * @{
 *
 * @file
 * @brief       High-level driver for the XBee S1 802.15.4 modem
 *
 * @author      Kévin Roussel <kevin.roussel@inria.fr>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef XBEE_H_
#define XBEE_H_

#include <stdint.h>

#include "kernel.h"
#include "mutex.h"
#include "periph/uart.h"
#include "periph/gpio.h"
#include "net/ng_netbase.h"
#include "net/ng_ieee802154.h"

#ifdef __cplusplus
 extern "C" {
#endif

/**
 * @brief   Maximum payload length that can be send
 */
#define XBEE_MAX_PAYLOAD_LENGTH     (100U)

/**
 * @brief   Maximum packet length, including XBee API frame overhead
 */
#define XBEE_MAX_PKT_LENGTH         (115U)

/**
 * @brief   Maximum length of a command response
 */
#define XBEE_MAX_RESP_LENGTH        (16U)

/**
 * @brief   Default protocol for data that is coming in
 */
#ifdef MODULE_NG_SIXLOWPAN
#define XBEE_DEFAULT_PROTOCOL       (NG_NETTYPE_SIXLOWPAN)
#else
#define XBEE_DEFAULT_PROTOCOL       (NG_NETTYPE_UNDEF)
#endif

/**
 * @brief   Default PAN ID used after initialization
 */
#define XBEE_DEFAULT_PANID          (0x0023)

/**
 * @brief   Default channel used after initialization
 */
#define XBEE_DEFAULT_CHANNEL        (26U)

/**
 * @name    Address flags
 * @{
 */
/**
 * @brief   Use long addresses if not otherwise defined when set, use short
 *          addresses when unset.
 */
#define XBEE_ADDR_FLAGS_LONG              (0x80)
/**
 * @}
 */

/**
 * @brief   States of the internal FSM for handling incoming UART frames
 *
 * Incoming data frames on the UART interfaces are handled using a finite state
 * machine (FSM) in the UARTs RX interrupt handler. The FSM is needed to extract
 * frame specific data as the frame size, frame type, and checksums.
 */
typedef enum {
    XBEE_INT_STATE_IDLE,        /**< waiting for the beginning of a new frame */
    XBEE_INT_STATE_SIZE1,       /**< waiting for the first byte (MSB) of the
                                 *   frame size field */
    XBEE_INT_STATE_SIZE2,       /**< waiting for the second byte (LSB) of the
                                 *   frame size field */
    XBEE_INT_STATE_TYPE,        /**< waiting for the frame type field */
    XBEE_INT_STATE_RESP,        /**< handling incoming data for AT command
                                 *   responses */
    XBEE_INT_STATE_RX,          /**< handling incoming data when receiving radio
                                 *   packets */
} xbee_rx_state_t;

/**
 * @brief   XBee device descriptor
 */
typedef struct {
    /* netdev fields */
    ng_netdev_driver_t const *driver;   /**< pointer to the devices interface */
    ng_netdev_event_cb_t event_cb;      /**< netdev event callback */
    kernel_pid_t mac_pid;               /**< the driver's thread's PID */
    /* device driver specific fields */
    uart_t uart;                        /**< UART interfaced used */
    gpio_t reset_pin;                   /**< GPIO pin connected to RESET */
    gpio_t sleep_pin;                   /**< GPIO pin connected to SLEEP */
    ng_nettype_t proto;                 /**< protocol the interface speaks */
    uint8_t options;                    /**< options field */
    uint8_t addr_flags;                 /**< address flags as defined above */
    uint8_t addr_short[2];              /**< onw 802.15.4 short address */
    eui64_t addr_long;                  /**< own 802.15.4 long address */
    /* general variables for the UART RX state machine */
    xbee_rx_state_t int_state;          /**< current state if the UART RX FSM */
    uint16_t int_size;                  /**< temporary space for parsing the
                                         *   frame size */
    /* values for the UART TX state machine */
    mutex_t tx_lock;                    /**< mutex to allow only one
                                         *   transmission at a time */
    uint8_t tx_buf[XBEE_MAX_PKT_LENGTH];/**< transmit data buffer */
    uint16_t tx_count;                  /**< counter for ongoing transmission */
    uint16_t tx_limit;                  /**< size of TX frame transferred */
    /* buffer and synchronization for command responses */
    mutex_t resp_lock;                  /**< mutex for waiting for AT command
                                         *   response frames */
    uint8_t resp_buf[XBEE_MAX_RESP_LENGTH]; /**< AT response data buffer */
    uint16_t resp_count;                /**< counter for ongoing transmission */
    uint16_t resp_limit;                /**< size RESP frame in transferred */
    /* buffer and synchronization for incoming network packets */
    uint8_t rx_buf[XBEE_MAX_PKT_LENGTH];/**< receiving data buffer */
    uint16_t rx_count;                  /**< counter for ongoing transmission */
    uint16_t rx_limit;                  /**< size RX frame transferred */
} xbee_t;

/**
 * @brief   Reference to the XBee driver interface
 */
extern const ng_netdev_driver_t xbee_driver;

/**
 * @brief   Initialize the given Xbee device
 *
 * @param[out] dev          Xbee device to initialize
 * @param[in]  uart         UART interfaced the device is connected to
 * @param[in]  baudrate     baudrate to use
 * @param[in]  sleep_pin    GPIO pin that is connected to the SLEEP pin, set to
 *                          GPIO_UNDEF if not used
 * @param[in]  status_pin   GPIO pin that is connected to the STATUS pin, set to
 *                          GPIO_UNDEF if not used
 *
 * @return                  0 on success
 * @return                  -ENODEV on invalid device descriptor
 * @return                  -ENXIO on invalid UART or GPIO pins
 */
int xbee_init(xbee_t *dev, uart_t uart, uint32_t baudrate,
              gpio_t sleep_pin, gpio_t status_pin);

/**
 * @brief   auto_init struct holding Xbee device initalization params
 */
typedef struct xbee_params {
    uart_t uart;            /**< UART interfaced the device is connected to */
    uint32_t baudrate;      /**< baudrate to use */
    gpio_t sleep_pin;       /**< GPIO pin that is connected to the SLEEP pin
                                 set to GPIO_UNDEF if not used */
    gpio_t status_pin;      /**< GPIO pin that is connected to the STATUS pin
                                 set to GPIO_UNDEF if not used */
} xbee_params_t;

#ifdef __cplusplus
}
#endif

#endif /* XBEE_H_ */
/** @} */
