/**
  * cc2420_rx.c - Implementation of receiving cc2420 functions.
  * Copyright (C) 2013 Milan Babel <babel@inf.fu-berlin.de>
  * Copyright (C) 2014 Kévin Roussel <Kevin.Roussel@inria.fr>
  *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
  */

#include <stdio.h>

#include "cc2420.h"
#include "cc2420_settings.h"
#include "cc2420_arch.h"
#include "cc2420_spi.h"
#include "ieee802154_frame.h"

#include "kernel_types.h"
#include "transceiver.h"
#include "msg.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

/* circular buffer for incoming 802.15.4 packets */
cc2420_packet_t cc2420_rx_buffer[CC2420_RX_BUF_SIZE];
volatile uint8_t rx_buffer_next;  /* index of next free cell in RX buffer */

/* pointer to the callback low-level function for packet reception */
typedef struct {
    enum { CC2420_CB_TYPE_NONE = 0, CC2420_CB_TYPE_RAW, CC2420_CB_TYPE_DATA } type;
    union {
        netdev_802154_raw_packet_cb_t raw;
        netdev_rcv_data_cb_t data;
    } cb;
} cc2420_callback_t;

static cc2420_callback_t recv_func = {0, {NULL}};

int cc2420_add_raw_recv_callback(netdev_t *dev, netdev_802154_raw_packet_cb_t recv_cb)
{
    /* XXX: first check only for backwards compatibility with transceiver
     *      (see cc2420_init) remove when adapter for transceiver exists */
    if (dev != &cc2420_netdev) {
        return -ENODEV;
    }

    if (recv_func.cb.raw != NULL) {
        return -ENOBUFS;
    }

    recv_func.type = CC2420_CB_TYPE_RAW;
    recv_func.cb.raw = recv_cb;

    return 0;
}

int cc2420_rem_raw_recv_callback(netdev_t *dev, netdev_802154_raw_packet_cb_t recv_cb)
{
    /* XXX: first check only for backwards compatibility with transceiver
     *      (see cc2420_init) remove when adapter for transceiver exists */
    if (dev != &cc2420_netdev) {
        return -ENODEV;
    }

    if (recv_func.cb.raw == recv_cb) {
        recv_func.cb.raw = NULL;
    }

    return 0;
}

int cc2420_add_data_recv_callback(netdev_t *dev, netdev_rcv_data_cb_t recv_cb)
{
    /* XXX: first check only for backwards compatibility with transceiver
     *      (see cc2420_init) remove when adapter for transceiver exists */
    if (dev != &cc2420_netdev) {
        return -ENODEV;
    }

    if (recv_func.cb.data != NULL) {
        return -ENOBUFS;
    }

    recv_func.type = CC2420_CB_TYPE_DATA;
    recv_func.cb.data = recv_cb;

    return 0;
}

int cc2420_rem_data_recv_callback(netdev_t *dev, netdev_rcv_data_cb_t recv_cb)
{
    /* XXX: first check only for backwards compatibility with transceiver
     *      (see cc2420_init) remove when adapter for transceiver exists */
    if (dev != &cc2420_netdev) {
        return -ENODEV;
    }


    if (recv_func.cb.data == recv_cb) {
        recv_func.cb.data = NULL;
    }

    return 0;
}

void cc2420_rx_handler(void)
{
    uint8_t pkt_len, pkt_lqi;
    int8_t pkt_rssi;
    bool crc_ok;

    /* read length */
    cc2420_read_fifo(&pkt_len, 1);

    /* read packet's raw payload */
    uint8_t buf[pkt_len - 2];
    cc2420_read_fifo(buf, pkt_len - 2);

    /* read rssi, lqi and crc */
    cc2420_read_fifo((uint8_t *) &pkt_rssi, 1);
    cc2420_read_fifo(&pkt_lqi, 1);
    crc_ok = ((pkt_lqi & 0x80) != 0);
    pkt_lqi &= 0x7F;

    if (!crc_ok) {
        DEBUG("Got packet with invalid crc.\n");
        return;
    }

    /* low-level reception mechanism (for MAC layer, among others) */
    if (recv_func.type == CC2420_CB_TYPE_RAW && recv_func.cb.raw != NULL) {
        recv_func.cb.raw(NULL, buf, pkt_len - 2, pkt_rssi, pkt_lqi, crc_ok);
    }

    /* decode received packet */
    cc2420_rx_buffer[rx_buffer_next].length = pkt_len;
    cc2420_rx_buffer[rx_buffer_next].rssi = pkt_rssi;
    cc2420_rx_buffer[rx_buffer_next].lqi = pkt_lqi;
    cc2420_rx_buffer[rx_buffer_next].crc = (crc_ok ? 1 : 0);
    ieee802154_frame_read(buf,
                          &cc2420_rx_buffer[rx_buffer_next].frame,
                          cc2420_rx_buffer[rx_buffer_next].length);

    /* low-level data reception mechanism */
    if (recv_func.type == CC2420_CB_TYPE_DATA && recv_func.cb.data != NULL &&
        cc2420_rx_buffer[rx_buffer_next].frame.fcf.frame_type == 1) {
        recv_func.cb.data(NULL, cc2420_rx_buffer[rx_buffer_next].frame.src_addr,
                          (cc2420_rx_buffer[rx_buffer_next].frame.fcf.src_addr_m == 2) ? 2 : 8,
                          cc2420_rx_buffer[rx_buffer_next].frame.dest_addr,
                          (cc2420_rx_buffer[rx_buffer_next].frame.fcf.dest_addr_m == 2) ? 2 : 8,
                          cc2420_rx_buffer[rx_buffer_next].frame.payload,
                          cc2420_rx_buffer[rx_buffer_next].frame.payload_len);
    }

    /* follow-up to transceiver module if adequate */
    if (cc2420_rx_buffer[rx_buffer_next].frame.fcf.frame_type != IEEE_802154_ACK_FRAME) {
#if ENABLE_DEBUG
        ieee802154_frame_print_fcf_frame(&cc2420_rx_buffer[rx_buffer_next].frame);
#endif

#ifdef MODULE_TRANSCEIVER

        /* notify transceiver thread if any */
        if (transceiver_pid != KERNEL_PID_UNDEF) {
            msg_t m;
            m.type = (uint16_t) RCV_PKT_CC2420;
            m.content.value = rx_buffer_next;
            msg_send_int(&m, transceiver_pid);
        }

#endif
    }

#if ENABLE_DEBUG
    else {
        DEBUG("GOT ACK for SEQ %u\n", cc2420_rx_buffer[rx_buffer_next].frame.seq_nr);
        ieee802154_frame_print_fcf_frame(&cc2420_rx_buffer[rx_buffer_next].frame);
    }
#endif

    /* shift to next buffer element */
    if (++rx_buffer_next == CC2420_RX_BUF_SIZE) {
        rx_buffer_next = 0;
    }
}
