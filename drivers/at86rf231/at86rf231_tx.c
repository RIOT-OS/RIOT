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
#include "vtimer.h"

#define ENABLE_DEBUG (0)
#include "debug.h"

static void at86rf231_xmit(uint8_t *data, radio_packet_length_t length);

static void at86rf231_gen_pkt(uint8_t *buf, at86rf231_packet_t *packet);

static uint8_t sequence_nr;

int16_t at86rf231_send(at86rf231_packet_t *packet)
{
    // Set missing frame information
    packet->frame.fcf.frame_ver = 0;

    if (packet->frame.src_pan_id == packet->frame.dest_pan_id) {
        packet->frame.fcf.panid_comp = 1;
    }
    else {
        packet->frame.fcf.panid_comp = 0;
    }

    if (packet->frame.fcf.src_addr_m == 2) {
        packet->frame.src_addr[0] = (uint8_t)(at86rf231_get_address() >> 8);
        packet->frame.src_addr[1] = (uint8_t)(at86rf231_get_address() & 0xFF);
    }
    else if (packet->frame.fcf.src_addr_m == 3) {
        packet->frame.src_addr[0] = (uint8_t)(at86rf231_get_address_long() >> 56);
        packet->frame.src_addr[1] = (uint8_t)(at86rf231_get_address_long() >> 48);
        packet->frame.src_addr[2] = (uint8_t)(at86rf231_get_address_long() >> 40);
        packet->frame.src_addr[3] = (uint8_t)(at86rf231_get_address_long() >> 32);
        packet->frame.src_addr[4] = (uint8_t)(at86rf231_get_address_long() >> 24);
        packet->frame.src_addr[5] = (uint8_t)(at86rf231_get_address_long() >> 16);
        packet->frame.src_addr[6] = (uint8_t)(at86rf231_get_address_long() >> 8);
        packet->frame.src_addr[7] = (uint8_t)(at86rf231_get_address_long() & 0xFF);
    }

    packet->frame.src_pan_id = at86rf231_get_pan();
    packet->frame.seq_nr = sequence_nr;

    sequence_nr += 1;

    // calculate size of the frame (payload + FCS) */
    packet->length = ieee802154_frame_get_hdr_len(&packet->frame) +
                     packet->frame.payload_len + 1;

    if (packet->length > AT86RF231_MAX_PKT_LENGTH) {
        return -1;
    }

    // FCS is added in hardware
    uint8_t pkt[packet->length];

    /* generate pkt */
    at86rf231_gen_pkt(pkt, packet);

    // transmit packet
    at86rf231_xmit(pkt, packet->length);
    return packet->length;
}

static void at86rf231_xmit(uint8_t *data, radio_packet_length_t length)
{
    // Go to state PLL_ON
    at86rf231_reg_write(AT86RF231_REG__TRX_STATE, AT86RF231_TRX_STATE__PLL_ON);

    // wait until it is on PLL_ON state
    uint8_t status;
    uint8_t max_wait = 100;   // TODO : move elsewhere, this is in 10us

    do {
        status = at86rf231_get_status();

        vtimer_usleep(10);

        if (!--max_wait) {
            printf("at86rf231 : ERROR : could not enter PLL_ON mode");
            break;
        }
    }
    while ((status & AT86RF231_TRX_STATUS_MASK__TRX_STATUS) != AT86RF231_TRX_STATUS__PLL_ON);

    /* radio driver state: sending */
    /* will be freed in at86rf231_rx_irq when TRX_END interrupt occurs */
    driver_state = AT_DRIVER_STATE_SENDING;

    // copy the packet to the radio FIFO
    at86rf231_write_fifo(data, length);
    DEBUG("Wrote to FIFO\n");

    // Start TX
    at86rf231_reg_write(AT86RF231_REG__TRX_STATE, AT86RF231_TRX_STATE__TX_START);
    DEBUG("Started TX\n");
}

/**
 * @brief Static function to generate byte array from at86rf231 packet.
 *
 */
static void at86rf231_gen_pkt(uint8_t *buf, at86rf231_packet_t *packet)
{
    uint8_t index, offset;
    index = ieee802154_frame_init(&packet->frame, &buf[1]);

    // add length for at86rf231
    buf[0] = packet->length + 1;
    index++;

    offset = index;

    while (index < packet->length) {
        buf[index] = packet->frame.payload[index - offset];
        index += 1;
    }
}
