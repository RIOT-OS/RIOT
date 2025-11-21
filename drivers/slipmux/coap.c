/*
 * SPDX-FileCopyrightText: 2025 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @{
 *
 * @file
 * @author  Bennet Hattesen
 */

#include "chunked_ringbuffer.h"
#include "checksum/crc16_ccitt.h"
#include "event.h"
#include "net/nanocoap.h"

#include "slipmux_internal.h"
#include "slipmux.h"

/* The special init is the result of normal fcs init combined with slipmux config start (0xa9) */
#define SPECIAL_INIT_FCS (0x374cU)

#define COAP_STACKSIZE (1024)
static char coap_stack[COAP_STACKSIZE];

static event_queue_t queue;

static uint8_t buf[CONFIG_SLIPMUX_COAP_BUFSIZE];

/* called in ISR context */
void slipmux_coap_notify(slipmux_t *dev)
{
    if (queue.waiter) {
        event_post(&queue, &dev->event);
    }
}

void slipmux_coap_send(uint8_t *buf, size_t len, slipmux_t *dev)
{
    uint16_t fcs_sum = crc16_ccitt_fcs_finish(SPECIAL_INIT_FCS, buf, len);

    slipmux_lock(dev);
    slipmux_write_byte(dev->config.uart, SLIPMUX_COAP_START);
    slipmux_write_bytes(dev->config.uart, buf, len);
    slipmux_write_bytes(dev->config.uart, (uint8_t *) &fcs_sum, 2);
    slipmux_write_byte(dev->config.uart, SLIPMUX_END);
    slipmux_unlock(dev);
}

int slipmux_coap_recv(uint8_t *buf, size_t buf_size, slipmux_t *dev)
{
    size_t len;
    if (crb_get_chunk_size(&dev->coap_rb, &len)) {
        if ((len > buf_size) || (len <= 2)) {
            return -1;
        }

        crb_consume_chunk(&dev->coap_rb, buf, len);

        /* Is the CRC correct via residue(=0xF0B8) test */
        if (crc16_ccitt_fcs_update(SPECIAL_INIT_FCS, buf, len) != 0xF0B8) {
            return -1;
        }

        /* cut off the FCS checksum at the end */
        size_t pktlen = len - 2;

        return pktlen;
    }
    return 0;
}

void slipmux_recv_handler(event_t * event)
{
    slipmux_t *dev = container_of(event, slipmux_t, event);

    while (1) {
        ssize_t len = slipmux_coap_recv(buf, sizeof(buf), dev);
        if (len <= 0) {
            return;
        }

        coap_pkt_t pkt;
        sock_udp_ep_t remote;
        coap_request_ctx_t ctx = {
            .remote = &remote,
        };
        if (coap_parse(&pkt, buf, len) < 0) {
            break;
        }

        unsigned int res = 0;
        if ((res = coap_handle_req(&pkt, buf, sizeof(buf), &ctx)) <= 0) {
            break;
        }

        slipmux_coap_send(buf, res, dev);
    }
}

static void *_coap_server_thread(void *arg)
{
    (void) arg;
    event_queue_claim(&queue);
    while (1) {
        event_t *event = event_wait(&queue);
        event->handler(event);
    }

    return NULL;
}

void slipmux_coap_init(slipmux_t *dev, unsigned index)
{
    (void) index;
    dev->event.handler = slipmux_recv_handler;
    crb_init(&dev->coap_rb, dev->coap_rx, sizeof(dev->coap_rx));

    if (!queue.waiter) {
        event_queue_init_detached(&queue);
        dev->coap_server_pid = thread_create(coap_stack, sizeof(coap_stack),
                                        THREAD_PRIORITY_MAIN - 1,
                                        THREAD_CREATE_STACKTEST, _coap_server_thread,
                                        (void *)dev, "Slipmux CoAP server");
    }
}

/** @} */
