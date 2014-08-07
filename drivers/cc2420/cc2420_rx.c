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
#include "debug.h"

/* circular buffer for incoming 802.15.4 packets */
cc2420_packet_t cc2420_rx_buffer[CC2420_RX_BUF_SIZE];
volatile uint8_t rx_buffer_next;  /* index of next free cell in RX buffer */

/* pointer to the callback low-level function for packet reception */
static receive_802154_packet_callback_t recv_func = NULL;


void cc2420_set_recv_callback(receive_802154_packet_callback_t recv_cb)
{
    recv_func = recv_cb;
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
    if (recv_func != NULL) {
        recv_func(buf, pkt_len - 2, pkt_rssi, pkt_lqi, crc_ok);
    }

    /* decode received packet */
    cc2420_rx_buffer[rx_buffer_next].length = pkt_len;
    cc2420_rx_buffer[rx_buffer_next].rssi = pkt_rssi;
    cc2420_rx_buffer[rx_buffer_next].lqi = pkt_lqi;
    cc2420_rx_buffer[rx_buffer_next].crc = (crc_ok ? 1 : 0);
    ieee802154_frame_read(buf,
                          &cc2420_rx_buffer[rx_buffer_next].frame,
                          cc2420_rx_buffer[rx_buffer_next].length);

    /* follow-up to transceiver module if adequate */
    if (cc2420_rx_buffer[rx_buffer_next].frame.fcf.frame_type != 2) {
#ifdef DEBUG
        ieee802154_frame_print_fcf_frame(&cc2420_rx_buffer[rx_buffer_next].frame);
#endif

        /* notify transceiver thread if any */
        if (transceiver_pid != KERNEL_PID_UNDEF) {
            msg_t m;
            m.type = (uint16_t) RCV_PKT_CC2420;
            m.content.value = rx_buffer_next;
            msg_send_int(&m, transceiver_pid);
        }
    }

#ifdef DEBUG
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
