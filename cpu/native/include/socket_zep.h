/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup drivers_socket_zep  Socket-based ZEP
 * @ingroup  drivers_netdev
 * @brief    UDP socket-based IEEE 802.15.4 device over ZEP
 * @{
 *
 * @see @ref net_zep for protocol definitions
 *
 * This ZEP implementation can send a dummy HELLO packet on startup.
 * This is used to make dispatchers aware of the node.
 * To enable this behavior, add
 *
 * ```
 * USEMODULE += socket_zep_hello
 * ```
 *
 * to your Makefile.
 *
 * A ZEP dispatcher can just drop those packets (ZEP type 0xFF) if it
 * chooses to parse the ZEP header.
 *
 * The header of the HELLO packet will look like this:
 *
 *     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *     |         Preamble (EX)         |  Version (2)  |  Type  (255)  |
 *     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *     |                                                               |
 *     +                                                               +
 *     |                                                               |
 *     +                         Reserved (0)                         +
 *     |                                                               |
 *     +                                                               +
 *     |                                                               |
 *     +               +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *     |               |      'H'      |      'E'      |      'L'      |
 *     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *     |      'L'      |      'O'      |       0       |       0       |
 *     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *     |       0       |       0       |       0       |       0       |
 *     +-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+-+
 *
 */

/**
 * @file
 * @brief  Socket ZEP definitions
 * @author Martine Lenders <m.lenders@fu-berlin.de>
 */
#ifndef SOCKET_ZEP_H
#define SOCKET_ZEP_H

#include "net/netdev.h"
#include "net/netdev/ieee802154.h"
#include "net/ieee802154/radio.h"
#include "net/zep.h"
#include "ztimer.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   ZEP device initialization parameters
 */
typedef struct {
    char *local_addr;   /**< local address string */
    char *local_port;   /**< local address string */
    char *remote_addr;  /**< remote address string */
    char *remote_port;  /**< local address string */
} socket_zep_params_t;

/**
 * @brief   ZEP device RX state
 */
typedef enum {
    ZEPDEV_STATE_IDLE,      /**< ZEP is not doing anything, not receiving frames */
    ZEPDEV_STATE_RX_ON,     /**< ZEP is able to receive frames */
    ZEPDEV_STATE_RX_RECV,   /**< unprocessed frame is in RX buffer, rx stopped */
    ZEPDEV_STATE_TX,        /**< ZEP is currently sending a frame */
} zepdev_state_t;

/**
 * @brief   ZEP device state
 */
typedef struct {
    /**
     * @brief   command line parameters
     */
    const socket_zep_params_t *params;
    ztimer_t ack_timer;             /**< timer to send delayed ACK */
    int sock_fd;                    /**< socket fd */
    uint32_t seq;                   /**< ZEP sequence number */
    /**
     * @brief   Receive buffer
     */
    uint8_t rcv_buf[sizeof(zep_v2_data_hdr_t) + IEEE802154_FRAME_LEN_MAX];
    /**
     * @brief   Send buffer
     */
    uint8_t snd_buf[sizeof(zep_v2_data_hdr_t) + IEEE802154_FRAME_LEN_MAX];
    uint8_t snd_len;                /**< bytes to send */
    uint8_t rcv_len;                /**< bytes received */
    uint16_t pan_id;                /**< PAN ID of the ZEP network */
    uint16_t chan;                  /**< virtual radio channel */
    /**
     * @brief   Short address of the virtual radio
     */
    uint8_t addr_short[IEEE802154_SHORT_ADDRESS_LEN];
    /**
     * @brief   Long address of the virtual radio
     */
    uint8_t addr_long[IEEE802154_LONG_ADDRESS_LEN];
    ieee802154_filter_mode_t filter_mode;   /**< frame filter mode */
    zepdev_state_t state;                   /**< device state machine */
    bool send_hello;                        /**< send HELLO packet on connect */
} socket_zep_t;

/**
 * @brief   Setup socket_zep_t structure
 *
 * @param[in] dev       the preallocated socket_zep_t device handle to setup
 * @param[in] params    initialization parameters
 */
void socket_zep_setup(socket_zep_t *dev, const socket_zep_params_t *params);

/**
 * @brief Cleanup socket resources
 *
 * @param dev  the socket_zep device handle to cleanup
 */
void socket_zep_cleanup(socket_zep_t *dev);

/**
 * @brief   Setup socket ZEP in order to be used with the IEEE 802.15.4 Radio HAL
 *
 * @param[in] dev  pointer to the socket ZEP instance
 * @param[in] hal  pointer to the HAL descriptor associated to the device
 */
void socket_zep_hal_setup(socket_zep_t *dev, ieee802154_dev_t *hal);

#ifdef __cplusplus
}
#endif

#endif /* SOCKET_ZEP_H */
/** @} */
