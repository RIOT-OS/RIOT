/*
 * Copyright (C) 2016 Freie Universität Berlin
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @defgroup    netdev_socket_zep  Socket-based ZEP
 * @ingroup     netdev
 * @brief       UDP socket-based IEEE 802.15.4 device over ZEP
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

/* 127 - 25 as in at86rf2xx */
#define SOCKET_ZEP_FRAME_PAYLOAD_LEN    (102)   /**< maximum possible payload size */

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
 */
void socket_zep_setup(socket_zep_t *dev, const socket_zep_params_t *params);

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
