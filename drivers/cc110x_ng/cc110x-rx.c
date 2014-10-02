/*
 * Copyright (C) 2009 Freie Universität Berlin
 * Copyright (C) 2013 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_cc110x_ng
 * @{
 * @file        cc110x-rx.c
 * @brief       Functions for packet reception on cc110x
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @}
 */

#include "cc110x_ng.h"
#include "cc110x-internal.h"

#include "kernel_types.h"
#include "hwtimer.h"
#include "msg.h"
#include "transceiver.h"

#include "cpu-conf.h"
#include "cpu.h"

#ifdef MT_TRANSCEIVER_DBG_IGNORE
#include <stdio.h>
#include <string.h>
#define IGN_MAX     (10)

radio_address_t ignored_addr[IGN_MAX];
static uint8_t is_ignored(radio_address_t addr);
#endif

static uint8_t receive_packet_variable(uint8_t *rxBuffer, radio_packet_length_t length);
static uint8_t receive_packet(uint8_t *rxBuffer, radio_packet_length_t length);

rx_buffer_t cc110x_rx_buffer[RX_BUF_SIZE];          ///< RX buffer
volatile uint8_t rx_buffer_next;        ///< Next packet in RX queue

void cc110x_rx_handler(void)
{
    uint8_t res = 0;

    /* Possible packet received, RX -> IDLE (0.1 us) */
    rflags.CAA      = 0;
    rflags.MAN_WOR  = 0;
    cc110x_statistic.packets_in++;

    res = receive_packet((uint8_t *)&(cc110x_rx_buffer[rx_buffer_next].packet), sizeof(cc110x_packet_t));

    if (res) {
        /* If we are sending a burst, don't accept packets.
         * Only ACKs are processed (for stopping the burst).
         * Same if state machine is in TX lock. */
        if (radio_state == RADIO_SEND_BURST || rflags.TX) {
            cc110x_statistic.packets_in_while_tx++;
            return;
        }

        cc110x_rx_buffer[rx_buffer_next].rssi = rflags._RSSI;
        cc110x_rx_buffer[rx_buffer_next].lqi = rflags._LQI;
        cc110x_strobe(CC1100_SFRX);     /* ...for flushing the RX FIFO */

        /* Valid packet. After a wake-up, the radio should be in IDLE.
         * So put CC1100 to RX for WOR_TIMEOUT (have to manually put
         * the radio back to sleep/WOR). */
        //cc110x_spi_write_reg(CC1100_MCSM0, 0x08); /* Turn off FS-Autocal */
        cc110x_write_reg(CC1100_MCSM2, 0x07);   /* Configure RX_TIME (until end of packet) */
        cc110x_strobe(CC1100_SRX);
        hwtimer_wait(IDLE_TO_RX_TIME);
        radio_state = RADIO_RX;

#ifdef MT_TRANSCEIVER_DBG_IGNORE

        if (is_ignored(cc110x_rx_buffer[rx_buffer_next].packet.phy_src)) {
            LED_RED_TOGGLE;
            return;
        }

#endif

        /* notify transceiver thread if any */
        if (transceiver_pid != KERNEL_PID_UNDEF) {
            msg_t m;
            m.type = (uint16_t) MT_TRANSCEIVER_RCV_PKT_CC1100;
            m.content.value = rx_buffer_next;
            msg_send_int(&m, transceiver_pid);
        }

        /* shift to next buffer element */
        if (++rx_buffer_next == RX_BUF_SIZE) {
            rx_buffer_next = 0;
        }

        return;
    }
    else {
        /* No ACK received so TOF is unpredictable */
        rflags.TOF = 0;

        /* CRC false or RX buffer full -> clear RX FIFO in both cases */
        cc110x_strobe(CC1100_SIDLE);    /* Switch to IDLE (should already be)... */
        cc110x_strobe(CC1100_SFRX);     /* ...for flushing the RX FIFO */

        /* If packet interrupted this nodes send call,
         * don't change anything after this point. */
        if (radio_state == RADIO_AIR_FREE_WAITING) {
            cc110x_strobe(CC1100_SRX);
            hwtimer_wait(IDLE_TO_RX_TIME);
            return;
        }

        /* If currently sending, exit here (don't go to RX/WOR) */
        if (radio_state == RADIO_SEND_BURST) {
            cc110x_statistic.packets_in_while_tx++;
            return;
        }

        /* No valid packet, so go back to RX/WOR as soon as possible */
        cc110x_switch_to_rx();
    }
}


static uint8_t receive_packet_variable(uint8_t *rxBuffer, radio_packet_length_t length)
{
    uint8_t status[2];
    uint8_t packetLength = 0;

    /* Any bytes available in RX FIFO? */
    if ((cc110x_read_status(CC1100_RXBYTES) & BYTES_IN_RXFIFO)) {
        /* Read length byte (first byte in RX FIFO) */
        cc110x_read_fifo((char *) &packetLength, 1);

        /* Read data from RX FIFO and store in rxBuffer */
        if (packetLength <= length) {
            /* Put length byte at first position in RX Buffer */
            rxBuffer[0] = packetLength;

            /* Read the rest of the packet */
            //cc110x_readburst_reg(CC1100_RXFIFO, (char*)rxBuffer+1, packetLength);
            cc110x_read_fifo((char *) rxBuffer + 1, packetLength);

            /* Read the 2 appended status bytes (status[0] = RSSI, status[1] = LQI) */
            cc110x_readburst_reg(CC1100_RXFIFO, (char *)status, 2);

            /* Store RSSI value of packet */
            rflags._RSSI = status[I_RSSI];

            /* MSB of LQI is the CRC_OK bit */
            rflags.CRC_STATE = (status[I_LQI] & CRC_OK) >> 7;

            if (!rflags.CRC_STATE) {
                cc110x_statistic.packets_in_crc_fail++;
            }

            /* Bit 0-6 of LQI indicates the link quality (LQI) */
            rflags._LQI = status[I_LQI] & LQI_EST;

            return rflags.CRC_STATE;
        }
        /* too many bytes in FIFO */
        else {
            /* RX FIFO get automatically flushed if return value is false */
            return 0;
        }
    }
    /* no bytes in RX FIFO */
    else {
        /* RX FIFO get automatically flushed if return value is false */
        return 0;
    }
}

static uint8_t receive_packet(uint8_t *rxBuffer, radio_packet_length_t length)
{
    uint8_t pkt_len_cfg = cc110x_read_reg(CC1100_PKTCTRL0) & PKT_LENGTH_CONFIG;

    if (pkt_len_cfg == VARIABLE_PKTLEN) {
        return receive_packet_variable(rxBuffer, length);
    }

    /* Fixed packet length not supported. */
    /* RX FIFO get automatically flushed if return value is false */
    return 0;
}

#ifdef MT_TRANSCEIVER_DBG_IGNORE
void cc110x_init_ignore(void)
{
    memset(ignored_addr, 0, IGN_MAX * sizeof(radio_address_t));
}

uint8_t cc110x_add_ignored(radio_address_t addr)
{
    uint8_t i = 0;

    while ((i < IGN_MAX) && ignored_addr[i++]) {
        printf("i: %hu\n", i);
    }

    if (i > IGN_MAX) {
        return 0;
    }

    ignored_addr[i - 1] = addr;
    return 1;
}

static uint8_t is_ignored(radio_address_t addr)
{
    uint8_t i;

    for (i = 0; i < IGN_MAX; i++) {
        if (ignored_addr[i] == addr) {
            return 1;
        }
    }

    return 0;
}
#endif
