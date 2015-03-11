/*
 * Copyright (C) 2014 Freie Universität Berlin
 * Copyright (C) 2013 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_cc110x
 * @{
 * @file        cc110x-rx.c
 * @brief       Functions for packet reception and transmission on cc110x devices
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @author      Fabian Nack <nack@inf.fu-berlin.de>
 * @}
 */
#include <stdio.h>

#include "cc110x.h"
#include "cc110x-internal.h"

#include "periph/gpio.h"
#include "irq.h"

#include "kernel_types.h"
#include "hwtimer.h"
#include "msg.h"
#include "transceiver.h"

#include "cpu-conf.h"
#include "cpu.h"

#ifdef MODULE_NETDEV_BASE
#include "netdev/base.h"

netdev_rcv_data_cb_t cc110x_recv_cb = NULL;
#endif

/* Internal function prototypes */
static uint8_t receive_packet_variable(uint8_t *rxBuffer, radio_packet_length_t length);
static uint8_t receive_packet(uint8_t *rxBuffer, radio_packet_length_t length);

/* Global variables */
rx_buffer_t cc110x_rx_buffer[RX_BUF_SIZE];      /* RX buffer */
volatile uint8_t rx_buffer_next;                /* Next packet in RX queue */

void cc110x_rx_handler(void *args)
{
    uint8_t res = 0;

    /* Possible packet received, RX -> IDLE (0.1 us) */
    cc110x_statistic.packets_in++;

    res = receive_packet((uint8_t *)&(cc110x_rx_buffer[rx_buffer_next].packet),
            sizeof(cc110x_packet_t));

    if (res) {
        /* If we are sending a burst, don't accept packets.
         * Only ACKs are processed (for stopping the burst).
         * Same if state machine is in TX lock. */
        if (radio_state == RADIO_SEND_BURST) {
            cc110x_statistic.packets_in_while_tx++;
            return;
        }

        cc110x_rx_buffer[rx_buffer_next].rssi = rflags._RSSI;
        cc110x_rx_buffer[rx_buffer_next].lqi = rflags._LQI;
        cc110x_strobe(CC1100_SFRX);     /* ...for flushing the RX FIFO */

        /* Valid packet. After a wake-up, the radio should be in IDLE.
         * So put CC110x to RX for WOR_TIMEOUT (have to manually put
         * the radio back to sleep/WOR). */
        cc110x_write_reg(CC1100_MCSM2, 0x07);   /* Configure RX_TIME (until end of packet) */
        cc110x_strobe(CC1100_SRX);
        hwtimer_wait(IDLE_TO_RX_TIME);
        radio_state = RADIO_RX;

#ifdef MODULE_TRANSCEIVER
        /* notify transceiver thread if any */
        if (transceiver_pid != KERNEL_PID_UNDEF) {
            msg_t m;
            m.type = (uint16_t) RCV_PKT_CC1100;
            m.content.value = rx_buffer_next;
            msg_send_int(&m, transceiver_pid);
        }
#endif

#ifdef MODULE_NETDEV_BASE
        if (cc110x_recv_cb != NULL) {
            cc110x_packet_t p = cc110x_rx_buffer[rx_buffer_next].packet;
            cc110x_recv_cb(&cc110x_dev, &p.phy_src, sizeof(uint8_t), &p.address,
                    sizeof(uint8_t), p.data, p.length - CC1100_HEADER_LENGTH);
        }
#endif

        /* shift to next buffer element */
        if (++rx_buffer_next == RX_BUF_SIZE) {
            rx_buffer_next = 0;
        }

        return;
    }
    else {
        /* CRC false or RX buffer full -> clear RX FIFO in both cases */
        cc110x_strobe(CC1100_SIDLE);    /* Switch to IDLE (should already be)... */
        cc110x_strobe(CC1100_SFRX);     /* ...for flushing the RX FIFO */

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

    /* Any bytes available in RX FIFO? */
    if ((cc110x_read_status(CC1100_RXBYTES) & BYTES_IN_RXFIFO)) {
        uint8_t packetLength = 0;

        /* Read length byte (first byte in RX FIFO) */
        packetLength = cc110x_read_reg(CC1100_RXFIFO);

        /* Read data from RX FIFO and store in rxBuffer */
        if (packetLength <= length) {
	     uint8_t crc_ok = 0;

            /* Put length byte at first position in RX Buffer */
            rxBuffer[0] = packetLength;

            /* Read the rest of the packet */
            cc110x_readburst_reg(CC1100_RXFIFO, (char *) rxBuffer + 1, packetLength);

            /* Read the 2 appended status bytes (status[0] = RSSI, status[1] = LQI) */
            cc110x_readburst_reg(CC1100_RXFIFO, (char *)status, 2);

            /* Store RSSI value of packet */
            rflags._RSSI = status[I_RSSI];

            /* MSB of LQI is the CRC_OK bit */
            crc_ok = (status[I_LQI] & CRC_OK) >> 7;

            if (!crc_ok) {
                cc110x_statistic.packets_in_crc_fail++;
            }

            /* Bit 0-6 of LQI indicates the link quality (LQI) */
            rflags._LQI = status[I_LQI] & LQI_EST;

            return crc_ok;
        }
        /* too many bytes in FIFO */
        else {
            /* RX FIFO gets automatically flushed if return value is false */
            return 0;
        }
    }
    /* no bytes in RX FIFO */
    else {
        /* RX FIFO gets automatically flushed if return value is false */
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

int8_t cc110x_send(cc110x_packet_t *packet)
{
    volatile uint32_t abort_count;
    uint8_t size;

    radio_state = RADIO_SEND_BURST;

    /*
     * Number of bytes to send is:
     * length of phy payload (packet->length)
     * + size of length field (1 byte)
     */
    size = packet->length + 1;

    /* The number of bytes to be transmitted must be smaller
     * or equal to PACKET_LENGTH (62 bytes). So the receiver
     * can put the whole packet in its RX-FIFO (with appended
     * packet status bytes).*/
    if (size > PACKET_LENGTH) {
        return 0;
    }

    packet->phy_src = cc110x_get_address();

    /* Disable RX interrupt */
    gpio_irq_disable(CC110X_GDO2);

    /* Put CC110x in IDLE mode to flush the FIFO */
    cc110x_strobe(CC1100_SIDLE);
    /* Flush TX FIFO to be sure it is empty */
    cc110x_strobe(CC1100_SFTX);
    /* Write packet into TX FIFO */
    cc110x_writeburst_reg(CC1100_TXFIFO, (char *) packet, size);
    /* Switch to TX mode */
    abort_count = 0;
    unsigned int cpsr = disableIRQ();
    cc110x_strobe(CC1100_STX);

    /* Wait for GDO2 to be set -> sync word transmitted */
    while (gpio_read(CC110X_GDO2) == 0) {
        abort_count++;

        if (abort_count > CC1100_SYNC_WORD_TX_TIME) {
            /* Abort waiting. CC110x maybe in wrong mode */
            break;
        }
    }

    restoreIRQ(cpsr);

    /* Wait for GDO2 to be cleared -> end of packet */
    while (gpio_read(CC110X_GDO2) != 0);

    gpio_irq_enable(CC110X_GDO2);
    cc110x_statistic.raw_packets_out++;

    /* Go to RX mode after TX */
    cc110x_switch_to_rx();

    return size;
}
