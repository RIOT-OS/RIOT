/*
 * Copyright (C) 2015 PHYTEC Messtechnik GmbH
 *
 * This file is subject to the terms and conditions of the GNU Lesser General
 * Public License v2.1. See the file LICENSE in the top level directory for more
 * details.
 */

/**
 * @ingroup     drivers_kw2xrf
 * @{
 * @file        kw2xrf_rx.c
 * @brief       Receive functionality of kw2xrf driver
 *
 * @author      Johann Fischer <j.fischer@phytec.de>
 * @author      Jonas Remmert <j.remmert@phytec.de>
 * @}
 */

#include <stdio.h>

#include "kw2xrf.h"
#include "kw2xrf_reg.h"
#include "kw2xrf_spi.h"
#include "kw2xrf_internal.h"
#include "ieee802154_frame.h"

#include "kernel_types.h"
#include "msg.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

/* circular buffer for incoming 802.15.4 packets */
kw2xrf_packet_t kw2xrf_rx_buffer[MKW2XDRF_RX_BUF_SIZE];
volatile uint8_t rx_buffer_next;  /* index of next free cell in RX buffer */

/* pointer to the callback low-level function for packet reception */
typedef struct {
    enum { MKW2XDRF_CB_TYPE_NONE = 0, MKW2XDRF_CB_TYPE_RAW, MKW2XDRF_CB_TYPE_DATA } type;
    union {
        netdev_802154_raw_packet_cb_t raw;
        netdev_rcv_data_cb_t data;
    } cb;
} kw2xrf_callback_t;

/* pointer to the callback low-level function for packet reception */
static kw2xrf_callback_t recv_func = {0, {NULL}};

int kw2xrf_add_raw_recv_callback(netdev_t *dev, netdev_802154_raw_packet_cb_t recv_cb)
{
    /* XXX: first check only for backwards compatibility with transceiver
     *      (see kw2xrf_init) remove when adapter for transceiver exists */
    if (dev != &kw2xrf_netdev) {
        return -ENODEV;
    }

    if (recv_func.cb.raw != NULL) {
        return -ENOBUFS;
    }

    recv_func.type = MKW2XDRF_CB_TYPE_RAW;
    recv_func.cb.raw = recv_cb;

    return 0;
}

int kw2xrf_rem_raw_recv_callback(netdev_t *dev, netdev_802154_raw_packet_cb_t recv_cb)
{
    /* XXX: first check only for backwards compatibility with transceiver
     *      (see cc2420_init) remove when adapter for transceiver exists */
    if (dev != &kw2xrf_netdev) {
        return -ENODEV;
    }

    if (recv_func.cb.raw == recv_cb) {
        recv_func.cb.raw = NULL;
    }

    return 0;
}

int kw2xrf_add_data_recv_callback(netdev_t *dev, netdev_rcv_data_cb_t recv_cb)
{
    /* XXX: first check only for backwards compatibility with transceiver
     *      (see kw2xrf_init) remove when adapter for transceiver exists */
    if (dev != &kw2xrf_netdev) {
        return -ENODEV;
    }

    if (recv_func.cb.data != NULL) {
        return -ENOBUFS;
    }

    recv_func.type = MKW2XDRF_CB_TYPE_DATA;
    recv_func.cb.data = recv_cb;

    return 0;
}

int kw2xrf_rem_data_recv_callback(netdev_t *dev, netdev_rcv_data_cb_t recv_cb)
{
    /* XXX: first check only for backwards compatibility with transceiver
     *      (see kw2xrf_init) remove when adapter for transceiver exists */
    if (dev != &kw2xrf_netdev) {
        return -ENODEV;
    }


    if (recv_func.cb.data == recv_cb) {
        recv_func.cb.data = NULL;
    }

    return 0;
}

void kw2xrf_rx_handler(void)
{
    uint8_t pkt_len, pkt_lqi;
    int8_t pkt_rssi;
    bool crc_ok = true;
    uint8_t buf[MKW2XDRF_MAX_PKT_LENGTH + 1];

    pkt_len = kw2xrf_read_dreg(MKW2XDM_RX_FRM_LEN);

    /* read PSDU */
    kw2xrf_read_fifo(buf, pkt_len + 1);
    pkt_lqi = buf[pkt_len];
    pkt_rssi = 0;

    /* low-level reception mechanism (for MAC layer, among others)
    Note: CRC is checked automatically in the receiver hardware.
    Only valid packets will trigger an rx-interrupt. 
    Automatic checking could be switched off in PHY_CTRL2-Reg. CRC can then be found in 
    Modem_IRQSTS2 field in Reg CRCVALID */
    if ((recv_func.type == MKW2XDRF_CB_TYPE_RAW) && (recv_func.cb.raw != NULL)) {
        recv_func.cb.raw(NULL, buf, pkt_len - 2, pkt_rssi, pkt_lqi, crc_ok);
    }

    /* decode received packet */
    kw2xrf_rx_buffer[rx_buffer_next].length = pkt_len;
    kw2xrf_rx_buffer[rx_buffer_next].rssi = pkt_rssi;
    kw2xrf_rx_buffer[rx_buffer_next].lqi = pkt_lqi;
    /* As noted above, crc is checked in hardware */
    kw2xrf_rx_buffer[rx_buffer_next].crc = crc_ok;  
    ieee802154_frame_read(buf,
                          &kw2xrf_rx_buffer[rx_buffer_next].frame,
                          pkt_len);

    /* follow-up to transceiver module if adequate */
    if (kw2xrf_rx_buffer[rx_buffer_next].frame.fcf.frame_type != 2) {
#ifdef DEBUG
        ieee802154_frame_print_fcf_frame(&kw2xrf_rx_buffer[rx_buffer_next].frame);
#endif
    }

#ifdef MODULE_NETDEV_BASE

    //TODO: Test this!
    if (recv_func.cb.data != NULL) {
        kw2xrf_packet_t p = kw2xrf_rx_buffer[rx_buffer_next];
        recv_func.cb.data(&kw2xrf_netdev, &p.frame.src_addr, sizeof(p.frame.src_addr), &p.frame.dest_addr,
                          sizeof(p.frame.src_addr), p.frame.payload, p.frame.payload_len);
    }

#endif

#ifdef MODULE_TRANSCEIVER
        /* notify transceiver thread if any */
        if (transceiver_pid != KERNEL_PID_UNDEF) {
            msg_t m;
            m.type = (uint16_t) RCV_PKT_KW2XRF;
            m.content.value = rx_buffer_next;
            msg_send_int(&m, transceiver_pid);
        }
#endif

#ifdef DEBUG
    else {
        DEBUG("GOT ACK for SEQ %u\n", kw2xrf_rx_buffer[rx_buffer_next].frame.seq_nr);
        ieee802154_frame_print_fcf_frame(&kw2xrf_rx_buffer[rx_buffer_next].frame);
    }

#endif

    /* shift to next buffer element */
    if (++rx_buffer_next == MKW2XDRF_RX_BUF_SIZE) {
        rx_buffer_next = 0;
    }
}
/** @} */
