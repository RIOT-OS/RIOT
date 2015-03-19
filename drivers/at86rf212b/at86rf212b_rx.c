/*
 * Copyright (C) 2014 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_at86rf212b
 * @{
 *
 * @file
 * @brief       RX related functionality for the AT86RF212B device driver
 *
 * @author      Alaeddine Weslati <alaeddine.weslati@inria.fr>
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 * @author      Pon Nidhya Elango, TICET, IIT Bombay <nidhya.e@gmail.com>
 *
 * @}
 */

#include "at86rf212b.h"
#include "at86rf212b_spi.h"

#include "kernel_types.h"
#include "transceiver.h"
#include "msg.h"

#define ENABLE_DEBUG (0)
#if ENABLE_DEBUG
#define DEBUG_ENABLED
#endif
#include "debug.h"

#define PHY_RSSI_BASE_VAL_BPSK_20             (-100)
#define PHY_RSSI_BASE_VAL_BPSK_40             (-99)

at86rf212b_packet_t at86rf212b_rx_buffer[AT86RF212B_RX_BUF_SIZE];
static uint8_t buffer[AT86RF212B_RX_BUF_SIZE][AT86RF212B_MAX_PKT_LENGTH];
volatile uint8_t rx_buffer_next;
extern netdev_802154_raw_packet_cb_t at86rf212b_raw_packet_cb;

void at86rf212b_rx_handler(void)
{
    uint8_t lqi, fcs_rssi;
    /* Added for rssi measurement in Extended operating mode */
    int8_t rssi_ed;
    rssi_ed = at86rf212b_reg_read(AT86RF212B_REG__PHY_ED_LEVEL);

    /* read packet length */
    at86rf212b_read_fifo(&at86rf212b_rx_buffer[rx_buffer_next].length, 1);

    /* read psdu, read packet with length as first byte and lqi as last byte. */
    uint8_t *buf = buffer[rx_buffer_next];
    at86rf212b_read_fifo(buf, at86rf212b_rx_buffer[rx_buffer_next].length);

    /* read lqi which is appended after the psdu */
    lqi = buf[at86rf212b_rx_buffer[rx_buffer_next].length - 1];

    /* read fcs and rssi, from a register */
    fcs_rssi = at86rf212b_reg_read(AT86RF212B_REG__PHY_RSSI);

    /* build package*/
    at86rf212b_rx_buffer[rx_buffer_next].lqi = lqi;
    /* RSSI has no meaning here, it should be read during packet reception. */
    at86rf212b_rx_buffer[rx_buffer_next].rssi = fcs_rssi & 0x0F;  /* bit[4:0] */
    /* bit7, boolean, 1 FCS valid, 0 FCS not valid */
    at86rf212b_rx_buffer[rx_buffer_next].crc = (fcs_rssi >> 7) & 0x01;

    if (at86rf212b_rx_buffer[rx_buffer_next].crc == 0) {
        DEBUG("at86rf212b: Got packet with invalid crc.\n");
        return;
    }

#if ENABLE_DEBUG
    DEBUG("pkg: ");
    for (int i = 1; i < at86rf212b_rx_buffer[rx_buffer_next].length; i++) {
        DEBUG("%x ", buf[i]);
    }
    DEBUG("\n");
#endif

    /* read buffer into ieee802154_frame */
    ieee802154_frame_read(&buf[1], &at86rf212b_rx_buffer[rx_buffer_next].frame,
                          at86rf212b_rx_buffer[rx_buffer_next].length);

    /* if packet is no ACK */
    if (at86rf212b_rx_buffer[rx_buffer_next].frame.fcf.frame_type != IEEE_802154_ACK_FRAME) {
#ifdef ENABLE_DEBUG
        ieee802154_frame_print_fcf_frame(&at86rf212b_rx_buffer[rx_buffer_next].frame);
#endif
        if (at86rf212b_raw_packet_cb != NULL) {
            at86rf212b_raw_packet_cb(&at86rf212b_netdev, (void*)buf,
                                    at86rf212b_rx_buffer[rx_buffer_next].length,
                                    fcs_rssi, lqi, (fcs_rssi >> 7));
        }
#ifdef MODULE_TRANSCEIVER
        /* notify transceiver thread if any */
        if (transceiver_pid != KERNEL_PID_UNDEF) {
            msg_t m;
            m.type = (uint16_t) RCV_PKT_AT86RF212B;
            m.content.value = rx_buffer_next;
            msg_send_int(&m, transceiver_pid);
        }
#endif
    }
    else {
        /* This should not happen, ACKs are consumed by hardware */
#ifdef ENABLE_DEBUG
        DEBUG("GOT ACK for SEQ %u\n", at86rf212b_rx_buffer[rx_buffer_next].frame.seq_nr);
        ieee802154_frame_print_fcf_frame(&at86rf212b_rx_buffer[rx_buffer_next].frame);
#endif
    }

    /* shift to next buffer element */
    if (++rx_buffer_next == AT86RF212B_RX_BUF_SIZE) {
        rx_buffer_next = 0;
    }

}
