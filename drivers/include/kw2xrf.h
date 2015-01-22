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
 * @brief       Device driver for the Freescale MKW2xD radio
 * @{
 *
 * @file
 * @brief       Interface definition for the MKW2xD device driver
 *
 * @author     Johann Fischer <j.fischer@phytec.de>
 * @author     Jonas Remmert <j.remmert@phytec.de>
 */
/*
 * @brief packet buffer
 * <pre>
     buffer for tx            buffer for rx
     -----------------         -----------------
   0 | Frame Length  |       0 | PSDU byte 0   |
     -----------------         -----------------
   1 | PSDU byte 0   |       1 | PSDU byte 1   |
     -----------------         -----------------
   2 | PSDU byte 1   |       2 | PSDU byte 2   |
     -----------------         -----------------
 ... |               |     ... |               |
     -----------------         -----------------
 125 | PSDU byte 125 |     125 | FCS byte 0    |
     -----------------         -----------------
 126 | res. (FCS)    |     126 | FCS byte 1    |
     -----------------         -----------------
 127 | res. (FCS)    |     127 | LQI           |
     -----------------         -----------------
  </pre>
*/

#ifndef MKW2XDRF_H_
#define MKW2XDRF_H_

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "kernel_types.h"
#include "ieee802154_frame.h"

#include "netdev/802154.h"

#ifdef __cplusplus
extern "C" {
#endif

#define MKW2XDRF_RX_BUF_SIZE       3        /**< Max buffer size of kw2xrf rx buffer. */

#define MKW2XDRF_MAX_PKT_LENGTH    127      /**< Max packet length for kw2xrf device. */
#define MKW2XDRF_MAX_DATA_LENGTH   118      /**< Max data length in packet for kw2xrf device. */

#define MKW2XDRF_BROADCAST_ADDRESS (0xFFFF) /**< Broadcast address. */

#define MKW2XDRF_DEFAULT_CHANNR      11     /**< Default radio channel. */

/**
 *  Structure to represent a kw2xrf packet.
 */
typedef struct __attribute__((packed))
{
    /* @{ */
    uint8_t length;             /**< The length of the frame of the frame including fcs. */
    ieee802154_frame_t frame;   /**< The ieee802154 frame. */
    uint8_t lqi;                /**< The link quality indicator. */
    int8_t rssi;                /**< The rssi value. */
    bool crc;                   /**< 1 if crc was successfull, 0 otherwise. */
    /* @} */
}
kw2xrf_packet_t;

/**
 * Netdev representation of this driver.
 */
extern netdev_t kw2xrf_netdev;

/**
 * KW2XRF low-level radio driver definition.
 */
extern const netdev_802154_driver_t kw2xrf_driver;

#ifdef __cplusplus
}
#endif

#endif
/** @} */
