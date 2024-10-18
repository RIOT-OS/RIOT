/*
 * Copyright (C) 2014 INRIA
 * Copyright (C) 2015-2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_xbee XBee driver
 * @ingroup     drivers_netdev
 * @brief       High-level driver for the XBee S1 802.15.4 modem
 * @{
 *
 * @file
 * @brief       High-level driver for the XBee S1 802.15.4 modem
 *
 * @author      Kévin Roussel <kevin.roussel@inria.fr>
 * @author      Hauke Petersen <hauke.petersen@fu-berlin.de>
 */

#ifndef XBEE_H
#define XBEE_H

#include <stdint.h>

#include "mutex.h"
#include "periph/uart.h"
#include "periph/gpio.h"
#include "net/netdev.h"
#include "net/ieee802154.h"
#include "net/gnrc/nettype.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   Maximum payload length that can be send
 */
#ifdef MODULE_XBEE_ENCRYPTION
#define XBEE_MAX_PAYLOAD_LENGTH     (95U)
#else
#define XBEE_MAX_PAYLOAD_LENGTH     (100U)
#endif

/**
 * @brief   Maximum packet length, including XBee API frame overhead
 */
#define XBEE_MAX_PKT_LENGTH         (115U)

/**
 * @brief   Maximum length of a command response
 */
#define XBEE_MAX_RESP_LENGTH        (16U)

/**
 * @brief   Maximal possible size of a TX header
 */
#define XBEE_MAX_TXHDR_LENGTH       (14U)

/**
 * @brief   Default protocol for data that is coming in
 */
#ifdef MODULE_GNRC_SIXLOWPAN
#define XBEE_DEFAULT_PROTOCOL       (GNRC_NETTYPE_SIXLOWPAN)
#else
#define XBEE_DEFAULT_PROTOCOL       (GNRC_NETTYPE_UNDEF)
#endif

/**
 * @brief   Default PAN ID used after initialization
 */
#define XBEE_DEFAULT_PANID          (CONFIG_IEEE802154_DEFAULT_PANID)

/**
 * @brief   Default channel used after initialization
 */
#define XBEE_DEFAULT_CHANNEL        (CONFIG_IEEE802154_DEFAULT_CHANNEL)

/**
 * @name    Address flags
 * @{
 */
/**
 * @brief   Use long addresses if not otherwise defined when set, use short
 *          addresses when unset.
 */
#define XBEE_ADDR_FLAGS_LONG              (0x80)
/** @} */

/**
 * @brief   States of the internal FSM for handling incoming UART frames
 *
 * Incoming data frames on the UART interfaces are handled using a finite state
 * machine (FSM) in the UARTs RX interrupt handler. The FSM is needed to extract
 * frame specific data as the frame size, frame type, and checksums.
 */
typedef enum {
    XBEE_INT_STATE_IDLE,    /**< waiting for the beginning of a new frame */
    XBEE_INT_STATE_SIZE1,   /**< waiting for the first byte (MSB) of the
                             *   frame size field */
    XBEE_INT_STATE_SIZE2,   /**< waiting for the second byte (LSB) of the
                             *   frame size field */
    XBEE_INT_STATE_TYPE,    /**< waiting for the frame type field */
    XBEE_INT_STATE_RESP,    /**< handling incoming data for AT command
                             *   responses */
    XBEE_INT_STATE_RX,      /**< handling incoming data when receiving radio
                             *   packets */
} xbee_rx_state_t;

/**
 * @brief   Configuration parameters for XBee devices
 */
typedef struct {
    uart_t uart;            /**< UART interfaced the device is connected to */
    uint32_t br;            /**< baudrate to use */
    gpio_t pin_sleep;       /**< GPIO pin that is connected to the SLEEP pin
                                 set to GPIO_UNDEF if not used */
    gpio_t pin_reset;      /**< GPIO pin that is connected to the STATUS pin
                                 set to GPIO_UNDEF if not used */
} xbee_params_t;

/**
 * @brief   XBee device descriptor
 */
typedef struct {
    /* netdev fields */
    const struct netdev_driver *driver;     /**< ptr to that driver's interface. */
    netdev_event_cb_t event_callback;       /**< callback for device events */
    void* context;                          /**< ptr to network stack context */
    /* device driver specific fields */
    xbee_params_t p;                    /**< configuration parameters */
    uint8_t options;                    /**< options field */
    uint8_t addr_flags;                 /**< address flags as defined above */
    uint8_t addr_short[2];              /**< own 802.15.4 short address */
    eui64_t addr_long;                  /**< own 802.15.4 long address */
    /* general variables for the UART RX state machine */
    xbee_rx_state_t int_state;          /**< current state if the UART RX FSM */
    uint16_t int_size;                  /**< temporary space for parsing the
                                         *   frame size */
    /* values for the UART TX state machine */
    mutex_t tx_lock;                    /**< mutex to allow only one
                                         *   transmission at a time */
    uint8_t cmd_buf[XBEE_MAX_RESP_LENGTH];/**< command data buffer */
    uint8_t tx_fid;                     /**< TX frame ID */
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
 * @brief   Data structure for extraction L2 information of received packets
 */
typedef struct {
    uint8_t addr_len;                   /**< L2 address length (SRC and DST) */
    uint8_t bcast;                      /**< 0 := unicast, 1:=broadcast */
    uint8_t rssi;                       /**< RSSI value */
    uint8_t src_addr[8];                /**< L2 source address */
    uint8_t dst_addr[8];                /**< L2 dst address */
} xbee_l2hdr_t;

/**
 * @brief   Reference to the XBee driver interface
 */
extern const netdev_driver_t xbee_driver;

/**
 * @brief   Prepare the given Xbee device
 *
 * @param[out] dev          Xbee device to initialize
 * @param[in]  params       parameters for device initialization
 *
 * @return                  0 on success
 * @return                  -ENODEV on invalid device descriptor
 * @return                  -ENXIO on invalid UART or GPIO pins
 */
void xbee_setup(xbee_t *dev, const xbee_params_t *params);

/**
 * @brief   Put together the internal proprietary XBee header
 *
 * @param[in] dev           Xbee device to initialize
 * @param[out] xhdr         buffer to write the header into, MUST be at least of
 *                          length XBEE_MAX_TXHDR_LENGTH
 * @param[in]  payload_len  actual payload length (without the XBee header)
 * @param[in]  dst_addr     link layer (L2) destination address
 * @param[in]  addr_len     length of @p dst_addr in byte (MUST be 2 or 8)
 *
 * @return  the length of the created header in byte
 * @return  -EOVERFLOW if @p payload_len is greater than XBEE_MAX_PAYLOAD_LENGTH
 * @return  -ENOMSG if the given destination address has an invalid length
 */
int xbee_build_hdr(xbee_t *dev, uint8_t *xhdr, size_t payload_len,
                   void *dst_addr, size_t addr_len);

/**
 * @brief   Extract IEEE802.15.4 L2 header information from the XBee header
 *
 * @param[in] dev           Xbee device to initialize
 * @param[in]  xhdr         XBee header, starting with the API identifier
 * @param[out] l2hdr        the L2 header information is written here
 *
 * @return  the length of the XBee header
 * @return  -ENOMST if the given XBee header is invalid
 */
int xbee_parse_hdr(xbee_t *dev, const uint8_t *xhdr, xbee_l2hdr_t *l2hdr);

#ifdef __cplusplus
}
#endif

/** @} */
#endif /* XBEE_H */
