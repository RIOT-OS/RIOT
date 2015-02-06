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
#include "debug.h"

at86rf231_packet_t at86rf231_rx_buffer[AT86RF231_RX_BUF_SIZE];
static uint8_t buffer[AT86RF231_RX_BUF_SIZE][AT86RF231_MAX_PKT_LENGTH];
volatile uint8_t rx_buffer_next;
extern netdev_802154_raw_packet_cb_t at86rf231_raw_packet_cb;

/* this is run in interrupt context */
void _rx(void *arg)
{
    at86rf231_t *dev = (at86rf231_t *)arg;
    msg_t msg;

    msg.type = NETDEV_MSG_EVENT_TYPE;
    msg.content.value = AT86RF231_EVENT_RX;

    msg_send(&msg, dev->pid);
}

/* this is called from the netapi `event()` function */
void at86rf231_rx_handler(void)
{
    uint8_t length, tmp;
    pkt_t *ieee_frame, *payload;
    uint8_t *data;

    /* read length of received data */
    at86rf231_read_fifo(dev, &length, 1);

    /* allocate memory for ieee802.15.4 frame, payload and 2 pkt pointers */
    data = (uint8_t *)pktbuf_alloc((length - 1) +  (2 * sizeof(pkt_t)));
    if (data == NULL) {
        /* TODO: discard package - reason no memory buffer it */
        return;
    }
    ieee_frame = (pkt_t *)data;
    payload = (pkt_t *)(data + sizeof(pkt_t));

    /* get ieee802.15.4 frame */
    ieee_frame->payload = (data + (2 * sizeof(pkt_t)));
    ieee_frame->payload_proto = PKT_PROTO_IEEE802154;
    at86rf231_read_fifo(dev, ieee_frame->payload, length - 1);
    ieee_frame->payload_len = ieee802154_frame_get_hdr_len(
                                    (ieee802154_frame_t *)ieee_frame->payload);
    ieee_frame->headers = payload;

    /* mark payload inside frame */
    payload->payload_len = (size - 1) - ieee_frame->payload_len;
    payload->payload = ieee_frame->payload + ieee_frame->payload_len;
    payload->payload_proto = PKT_PROTO_UNKNOWN;
    payload->headers = NULL;

    /* read LQI */
    at86rf231_read_fifo(dev, &dev->lqi, 1);
    /* read RSSI and CRC status */
    tmp = at86rf231_reg_read(dev, AT86RF231_REG__PHY_RSSI);
    dev->rssi = tmp & 0x0f;
    dev->crc = tmp >> 7;

    if (dev->crc == 0) {
        DEBUG("at86rf231: Got packet with invalid crc.\n");
        return;
    }

    /* call receive data callback */
    dev->rcv_cb(ieee_frame);
    /* Read IRQ to clear it */
    at86rf231_reg_read(dev, AT86RF231_REG__IRQ_STATUS);
}
