/*
 * Copyright (C) 2014 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_at86rf231
 * @{
 *
 * @file
 * @brief       RX related functionality for the AT86RF231 device driver
 *
 * @author      Alaeddine Weslati <alaeddine.weslati@inria.fr>
 * @author      Thomas Eichinger <thomas.eichinger@fu-berlin.de>
 *
 * @}
 */

#include "at86rf231.h"
#include "at86rf231_spi.h"

#include "kernel_types.h"
#include "transceiver.h"
#include "msg.h"

#define ENABLE_DEBUG (0)
#if ENABLE_DEBUG
#define DEBUG_ENABLED
#endif
#include "debug.h"

at86rf231_packet_t at86rf231_rx_buffer[AT86RF231_RX_BUF_SIZE];
uint8_t buffer[AT86RF231_RX_BUF_SIZE][AT86RF231_MAX_PKT_LENGTH];
volatile uint8_t rx_buffer_next;

void at86rf231_rx_handler(void)
{
    uint8_t lqi, fcs_rssi;
    // read packet length
    at86rf231_read_fifo(&at86rf231_rx_buffer[rx_buffer_next].length, 1);

    // read psdu, read packet with length as first byte and lqi as last byte.
    uint8_t *buf = buffer[rx_buffer_next];
    at86rf231_read_fifo(buf, at86rf231_rx_buffer[rx_buffer_next].length);

    // read lqi which is appended after the psdu
    lqi = buf[at86rf231_rx_buffer[rx_buffer_next].length - 1];

    // read fcs and rssi, from a register
    fcs_rssi = at86rf231_reg_read(AT86RF231_REG__PHY_RSSI);

    // build package
    at86rf231_rx_buffer[rx_buffer_next].lqi = lqi;
    // RSSI has no meaning here, it should be read during packet reception.
    at86rf231_rx_buffer[rx_buffer_next].rssi = fcs_rssi & 0x0F;  // bit[4:0]
    // bit7, boolean, 1 FCS valid, 0 FCS not valid
    at86rf231_rx_buffer[rx_buffer_next].crc = (fcs_rssi >> 7) & 0x01;

    if (at86rf231_rx_buffer[rx_buffer_next].crc == 0) {
        DEBUG("Got packet with invalid crc.\n");
        return;
    }

    ieee802154_frame_read(&buf[1], &at86rf231_rx_buffer[rx_buffer_next].frame,
                          at86rf231_rx_buffer[rx_buffer_next].length);

    if (at86rf231_rx_buffer[rx_buffer_next].frame.fcf.frame_type != 2) {
#if DEBUG_ENABLED
        ieee802154_frame_print_fcf_frame(&at86rf231_rx_buffer[rx_buffer_next].frame);
#endif

        /* notify transceiver thread if any */
        if (transceiver_pid != KERNEL_PID_UNDEF) {
            msg_t m;
            m.type = (uint16_t) RCV_PKT_AT86RF231;
            m.content.value = rx_buffer_next;
            msg_send_int(&m, transceiver_pid);
        }
    }
    else {
#if DEBUG_ENABLED
        DEBUG("GOT ACK for SEQ %u\n", at86rf231_rx_buffer[rx_buffer_next].frame.seq_nr);
        ieee802154_frame_print_fcf_frame(&at86rf231_rx_buffer[rx_buffer_next].frame);
#endif
    }

    // shift to next buffer element
    if (++rx_buffer_next == AT86RF231_RX_BUF_SIZE) {
        rx_buffer_next = 0;
    }

    // Read IRQ to clear it
    at86rf231_reg_read(AT86RF231_REG__IRQ_STATUS);
}
