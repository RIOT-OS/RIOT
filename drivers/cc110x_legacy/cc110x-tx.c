/*
 * Copyright (C) 2009 Freie Universität Berlin
 * Copyright (C) 2013 INRIA
 *
 * This file is subject to the terms and conditions of the GNU Lesser
 * General Public License v2.1. See the file LICENSE in the top level
 * directory for more details.
 */

/**
 * @ingroup     drivers_cc110x_legacy
 * @{
 * @file
 * @brief       Functions for packet transmission on cc110x
 *
 * @author      Oliver Hahm <oliver.hahm@inria.fr>
 * @}
 */
#include <stdio.h>

#include "cc110x_legacy.h"
#include "cc110x-internal.h"

#include "irq.h"

int8_t cc110x_send(cc110x_packet_t *packet)
{
    volatile uint32_t abort_count;
    uint8_t size;
    /* TODO: burst sending */
    radio_state = RADIO_SEND_BURST;
    rflags.LL_ACK = 0;

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

    /* Disables RX interrupt etc. */
    cc110x_before_send();

    /* But CC1100 in IDLE mode to flush the FIFO */
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
    while (cc110x_get_gdo2() == 0) {
        abort_count++;

        if (abort_count > CC1100_SYNC_WORD_TX_TIME) {
            /* Abort waiting. CC1100 maybe in wrong mode */
            /* e.g. sending preambles for always */
            puts("[CC1100 TX] fatal error\n");
            break;
        }
    }

    restoreIRQ(cpsr);

    /* Wait for GDO2 to be cleared -> end of packet */
    while (cc110x_get_gdo2() != 0);


    /* Experimental - TOF Measurement */
    cc110x_after_send();
    cc110x_statistic.raw_packets_out++;

    /* Store number of transmission retries */
    rflags.TX = 0;

    /* Go to mode after TX (CONST_RX -> RX, WOR -> WOR) */
    cc110x_switch_to_rx();

    return size;
}
