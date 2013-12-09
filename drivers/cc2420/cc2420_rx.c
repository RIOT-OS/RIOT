/**
  * cc2420_rx.c - Implementation of receiving cc2420 functions.
  * Copyright (C) 2013 Milan Babel <babel@inf.fu-berlin.de>
  *
  * This source code is licensed under the GNU Lesser General Public License,
  * Version 2.  See the file LICENSE for more details.
  */

#include <stdio.h>

#include <cc2420.h>
#include <cc2420_settings.h>
#include <cc2420_arch.h>
#include <cc2420_spi.h>
#include <ieee802154_frame.h>

#include <transceiver.h>
#include <msg.h>
#include <debug.h>

cc2420_packet_t cc2420_rx_buffer[CC2420_RX_BUF_SIZE];
volatile uint8_t rx_buffer_next;

void cc2420_rx_handler(void)
{
    uint8_t rssi_crc_lqi[2];

    /* read length */
    cc2420_read_fifo(&cc2420_rx_buffer[rx_buffer_next].length, 1);

    /* read packet without rssi, crc and lqi */
    uint8_t buf[cc2420_rx_buffer[rx_buffer_next].length-2];
    cc2420_read_fifo(buf, cc2420_rx_buffer[rx_buffer_next].length-2);

    cc2420_swap_fcf_bytes(buf);
    /* read rssi, lqi and crc */
    cc2420_read_fifo(rssi_crc_lqi, 2);

    /* build package */
    cc2420_rx_buffer[rx_buffer_next].rssi = (int8_t)(rssi_crc_lqi[0]);
    cc2420_rx_buffer[rx_buffer_next].lqi = (uint8_t)(rssi_crc_lqi[1] & 0x7F);
    cc2420_rx_buffer[rx_buffer_next].crc = (uint8_t)((rssi_crc_lqi[1] & 0x80) >> 7);

    if(cc2420_rx_buffer[rx_buffer_next].crc == 0) {
        DEBUG("Got packet with invalid crc.\n");
        return;
    }
    ieee802154_frame_read(buf,
                          &cc2420_rx_buffer[rx_buffer_next].frame,
                          cc2420_rx_buffer[rx_buffer_next].length-2);
    if(cc2420_rx_buffer[rx_buffer_next].frame.fcf.frame_type != 2) {
#ifdef DEBUG
    ieee802154_frame_print_fcf_frame(&cc2420_rx_buffer[rx_buffer_next].frame);
#endif
    /* notify transceiver thread if any */
    if (transceiver_pid) {
        msg_t m;
        m.type = (uint16_t) RCV_PKT_CC2420;
        m.content.value = rx_buffer_next;
        msg_send_int(&m, transceiver_pid);
    }
    } else {
#ifdef DEBUG
        DEBUG("GOT ACK for SEQ %u\n", cc2420_rx_buffer[rx_buffer_next].frame.seq_nr);
        ieee802154_frame_print_fcf_frame(&cc2420_rx_buffer[rx_buffer_next].frame);
#endif
    }

    /* shift to next buffer element */
    if (++rx_buffer_next == CC2420_RX_BUF_SIZE) {
        rx_buffer_next = 0;
    }
}
