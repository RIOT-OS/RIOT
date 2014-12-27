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
#include <string.h>

#include "periph/gpio.h"
#include "irq.h"
#include "msg.h"
#include "kernel_types.h"
#include "transceiver.h"
#include "cpu-conf.h"

#include "cc110x.h"
#include "cc110x-internal.h"

#define ENABLE_DEBUG    (0)
#include "debug.h"

#ifdef MODULE_NETDEV_BASE
#include "netdev/base.h"

netdev_rcv_data_cb_t cc110x_recv_cb = NULL;
#endif

/* Internal variables*/
static cc110x_packet_t curr_packet;
static uint16_t bytes_read = 0;
static uint8_t bytes_rem = 0;

/* Global variables */
rx_buffer_t cc110x_rx_buffer[RX_BUF_SIZE];      /* RX buffer */
volatile uint8_t rx_buffer_next;                /* Next packet in RX queue */


/*------------------------------------------------------------------------------------*/
/*                                 CC110X RX/TX API                                   */
/*------------------------------------------------------------------------------------*/

int cc110x_send(cc110x_packet_t *packet)
{
    volatile uint32_t abort_count = 0;
    uint16_t bytes_left, bytes_send;
    char* packet_ptr = (char*) packet;

    /* Disable interrupts as they are only used for packet reception */
    gpio_irq_disable(CC110X_GDO0);
    gpio_irq_disable(CC110X_GDO2);

    radio_state = RADIO_SEND_BURST;

    /* Number of bytes to send is: packet->length + size of length field (1 byte) */
    bytes_left = packet->length + 1;

    /* Set the configured address as source address */
    packet->phy_src = cc110x_get_address();

    /* Put CC110x in IDLE mode to flush the FIFO (to be sure it is empty) */
    cc110x_strobe(CC1100_SIDLE);
    cc110x_strobe(CC1100_SFTX);

    if (bytes_left <= CC1100_FIFO_SIZE) {
        /* We can put the whole packet into the TX FIFO */
        cc110x_writeburst_reg(CC1100_TXFIFO, packet_ptr, bytes_left);
        bytes_send = bytes_left;

        /* Start transmission */
        unsigned int cpsr = disableIRQ();
        cc110x_strobe(CC1100_STX);
        /* Wait for sync word transmission */
        while (!gpio_read(CC110X_GDO2)) {
            abort_count++;
            if (abort_count > CC1100_SYNC_WORD_TX_TIME) {
                break;
            }
        }
        restoreIRQ(cpsr);
    }
    else {
        /* Packet is too large, we need to fill the TX FIFO multiple times */
        /* Push first 64 bytes of the packet */
        cc110x_writeburst_reg(CC1100_TXFIFO, packet_ptr, CC1100_FIFO_SIZE);
        bytes_send = CC1100_FIFO_SIZE;
        bytes_left -= CC1100_FIFO_SIZE;

        /* Start transmission */
        unsigned int cpsr = disableIRQ();
        cc110x_strobe(CC1100_STX);

        /* Consecutively push remaining bytes to TX FIFO */
        while (bytes_left > 0) {
            /* Calculate free space in TX FIFO */
            uint8_t free_space = CC1100_FIFO_SIZE -
                    (cc110x_read_status(CC1100_TXBYTES) & BYTES_IN_TXFIFO);
            if (free_space > 0) {
                if (free_space > bytes_left) {
                    free_space = bytes_left;
                }
                cc110x_writeburst_reg(CC1100_TXFIFO, packet_ptr + bytes_send, free_space);
                bytes_send += free_space;
                bytes_left -= free_space;
            }
        }

        /* Wait for sync word transmission */
        while (!gpio_read(CC110X_GDO2)) {
            abort_count++;
            if (abort_count > CC1100_SYNC_WORD_TX_TIME) {
                /* Sync word was already send, we missed it */
                break;
            }
        }
        restoreIRQ(cpsr);
    }

    if (cc110x_read_status(CC1100_TXBYTES) & TXFIFO_UNDERFLOW) {
        DEBUG("[CC110X TX] TXFIFO UNDERFLOW ERROR");
        /* Flush TX FIFO to be sure it is empty */
        cc110x_strobe(CC1100_SFTX);
    }

    /* Wait for TXFIFO to be emptied completely */
    while(cc110x_read_status(CC1100_TXBYTES) & BYTES_IN_TXFIFO);

    /* Wait for GDO2 to be cleared -> packet has been sent */
    while (gpio_read(CC110X_GDO2) != 0);
    cc110x_statistic.raw_packets_out++;

    /* Re-enable packet reception interrupts */
    gpio_irq_enable(CC110X_GDO0);
    gpio_irq_enable(CC110X_GDO2);

    /* Go to RX mode after TX */
    cc110x_switch_to_rx();

    return bytes_send;
}

void cc110x_rxfifo_of_handler(void)
{
    /* Disable interrupt (we only want it to trigger on reception start) */
    gpio_irq_disable(CC110X_GDO0);

    /* Read length byte from RX FIFO */
    bytes_rem = cc110x_read_reg(CC1100_RXFIFO);
    curr_packet.length = bytes_rem;
    bytes_read = 1;

    /* Read remaining bytes consecutively from RX FIFO */
    while (bytes_rem > 0) {
        uint8_t bytes_avail = (cc110x_read_status(CC1100_RXBYTES) & BYTES_IN_RXFIFO);
        /* Don't empty the FIFO completely */
        if (bytes_avail > 1) {
            if (bytes_avail <= bytes_rem) {
                cc110x_readburst_reg(CC1100_RXFIFO, ((char *)&curr_packet) + bytes_read,
                        bytes_avail - 1);
                bytes_rem -= (bytes_avail - 1);
                bytes_read += (bytes_avail - 1);
            }
            else {
                cc110x_readburst_reg(CC1100_RXFIFO, ((char *)&curr_packet) + bytes_read,
                        bytes_rem);
                bytes_read += bytes_rem;
                bytes_rem = 0;
            }
        }
    }
}

void cc110x_packet_end_handler(void)
{
    uint8_t status[2];

    /* Check whether a RXFIFO OVERFLOW occured */
    if (cc110x_read_status(CC1100_RXBYTES) & RXFIFO_OVERFLOW) {
        DEBUG("[CC110X] TXFIFO Overflow: Packet drop\n");
        /* Flush the RX FIFO to return to IDLE state */
        cc110x_strobe(CC1100_SFRX);
        bytes_read = 0;
        bytes_rem = 0;
        gpio_irq_enable(CC110X_GDO0);
        /* Switch back to RX state */
        cc110x_switch_to_rx();
        return;
    }

    /* Any bytes available in RX FIFO? */
    if ((cc110x_read_status(CC1100_RXBYTES) & BYTES_IN_RXFIFO)) {
        /* Possible packet received */
        cc110x_statistic.packets_in++;

        /* Check whether the packet was read completely (if not, it was probably invalid) */
        if ((bytes_read == 0) || (bytes_rem > 0)) {
            DEBUG("[CC110X] RX Problem: Packet drop\n");
            /* Switch to IDLE (should already be) for flushing the RX FIFO */
            cc110x_strobe(CC1100_SIDLE);
            cc110x_strobe(CC1100_SFRX);
            bytes_read = 0;
            bytes_rem = 0;
            gpio_irq_enable(CC110X_GDO0);
            /* Switch back to RX state */
            cc110x_switch_to_rx();
            return;
        }

        /* Read the 2 appended status bytes (status[0] = RSSI, status[1] = LQI) */
        cc110x_readburst_reg(CC1100_RXFIFO, (char *)status, 2);

        /* MSB of LQI is the CRC_OK bit */
        if ((status[I_LQI] & CRC_OK) >> 7) {
            cc110x_rx_buffer[rx_buffer_next].packet.length = curr_packet.length;
            cc110x_rx_buffer[rx_buffer_next].packet.flags = curr_packet.flags;
            cc110x_rx_buffer[rx_buffer_next].packet.phy_src = curr_packet.phy_src;
            cc110x_rx_buffer[rx_buffer_next].packet.address = curr_packet.address;
            memcpy(cc110x_rx_buffer[rx_buffer_next].packet.data,
                    curr_packet.data, (curr_packet.length - CC1100_HEADER_LENGTH));

            /* Store RSSI value of packet */
            cc110x_rx_buffer[rx_buffer_next].rssi = status[I_RSSI];
            /* Bit 0-6 of LQI indicates the link quality (LQI) */
            cc110x_rx_buffer[rx_buffer_next].lqi = status[I_LQI] & LQI_EST;

            /* Switch to IDLE (should already be) for flushing the RX FIFO */
            cc110x_strobe(CC1100_SIDLE);
            cc110x_strobe(CC1100_SFRX);

            /* Re-enable RXFIFO threshold interrupt */
            bytes_read = 0u;
            bytes_rem = 0u;
            gpio_irq_enable(CC110X_GDO0);

            /* Go back to RX state, to be ready for new packets */
            cc110x_write_reg(CC1100_MCSM2, 0x07);   /* Configure RX_TIME (until end of packet) */
            cc110x_switch_to_rx();

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
        }
        else {
            /* CRC false */
            DEBUG("[CC110X] CRC Error: Packet drop\n");
            cc110x_statistic.packets_in_crc_fail++;

            /* Switch to IDLE (should already be) for flushing the RX FIFO */
            cc110x_strobe(CC1100_SIDLE);
            cc110x_strobe(CC1100_SFRX);

            /* Re-enable RXFIFO threshold interrupt */
            bytes_read = 0u;
            bytes_rem = 0u;
            gpio_irq_enable(CC110X_GDO0);

            /* No valid packet, so go back to RX as soon as possible */
            cc110x_switch_to_rx();
        }
    }
}
