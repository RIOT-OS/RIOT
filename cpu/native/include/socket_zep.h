/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    drivers_socket_zep  Socket-based ZEP
 * @ingroup     drivers_netdev
 * @brief       UDP socket-based IEEE 802.15.4 device over ZEP
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
 * @{
 *
 * @file
 * @brief       Socket ZEP definitions
 *
 * @author  Martine Lenders <m.lenders@fu-berlin.de>
 */
#ifndef SOCKET_ZEP_H
#define SOCKET_ZEP_H

#include "net/netdev.h"
#include "net/netdev/ieee802154.h"
#include "net/zep.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief   ZEP device state
 */
typedef struct {
    netdev_ieee802154_t netdev;     /**< netdev internal member */
    int sock_fd;                    /**< socket fd */
    netdev_event_t last_event;      /**< event triggered */
    uint32_t seq;                   /**< ZEP sequence number */
    /**
     * @brief   Receive buffer
     */
    uint8_t rcv_buf[sizeof(zep_v2_data_hdr_t) + IEEE802154_FRAME_LEN_MAX];
    /**
     * @brief   Buffer for send header
     */
    uint8_t snd_hdr_buf[sizeof(zep_v2_data_hdr_t)];
    uint16_t chksum_buf;            /**< buffer for send checksum calculation */
} socket_zep_t;

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
 * @brief   Setup socket_zep_t structure
 *
 * @param[in] dev       the preallocated socket_zep_t device handle to setup
 * @param[in] params    initialization parameters
 * @param[in] index     index of @p params in a global parameter struct array.
 *                      If initialized manually, pass a unique identifier instead.
 */
void socket_zep_setup(socket_zep_t *dev, const socket_zep_params_t *params, uint8_t index);

/**
 * @brief Cleanup socket resources
 *
 * @param dev  the socket_zep device handle to cleanup
 */
void socket_zep_cleanup(socket_zep_t *dev);

#ifdef __cplusplus
}
#endif

#endif /* SOCKET_ZEP_H */
/** @} */
