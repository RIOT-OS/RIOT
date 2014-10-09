/**
 * internal nativenet transceiver interface
 *
 * Copyright (C) 2013 Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @{
 * @author  Ludwig Ortmann <ludwig.ortmann@fu-berlin.de>
 * @}
 */

#ifndef NATIVENET_INTERNAL_H
#define NATIVENET_INTERNAL_H

#include "tap.h"

#define NNEV_PWRDWN     0x01
#define NNEV_PWRUP      0x02
#define NNEV_MONITOR    0x03
#define NNEV_GETCHAN    0x04
#define NNEV_SETCHAN    0x05
#define NNEV_GETADDR    0x06
#define NNEV_SETADDR    0x07
#define NNEV_GETPAN     0x08
#define NNEV_SETPAN     0x09
#define NNEV_SEND       0x0a
#define NNEV_SWTRX      0x0b
#define NNEV_MAXEV      0x0b

#define _NATIVENET_DEV_MORE(dev) ((_nativenet_netdev_more_t *)dev->more)

struct rx_buffer_s {
    radio_packet_t packet;
    char data[NATIVE_MAX_DATA_LENGTH];
};

extern struct rx_buffer_s _nativenet_rx_buffer[RX_BUF_SIZE];

/**
 * @brief   Definition of network device data.
 */
typedef struct {
    /**
     * @brief   The channel assigned to this device
     *
     * @note    For internal use only, do not change externally!
     *
     * @internal
     */
    uint8_t _channel;

    /**
     * @brief   The PAN ID assigned to this device
     *
     * @note    For internal use only, do not change externally!
     * @internal
     */
    uint16_t _pan_id;

    /**
     * @brief   The short address assigned to this device
     *
     * @note    For internal use only, do not change externally!
     * @internal
     */
    radio_address_t _radio_addr;

    /**
     * @brief   The long address assigned to this device
     *
     * @note    For internal use only, do not change externally!
     * @internal
     */
    uint64_t _long_addr;

    /**
     * @brief   Flag to determine if device is in promiscuous mode
     *
     * @note    For internal use only, do not change externally!
     * @internal
     */
    uint8_t _is_monitoring;

    /**
     * @brief   Receive data callbacks for this device
     */
    netdev_rcv_data_cb_t _callbacks[NATIVENET_DEV_CB_MAX];
} _nativenet_netdev_more_t;

/* internal counterpart to nativenet_default_dev */
extern _nativenet_netdev_more_t _nativenet_default_dev_more;

void _nativenet_handle_packet(radio_packet_t *packet);
int8_t send_buf(radio_packet_t *packet);
#endif /* NATIVENET_INTERNAL_H */
