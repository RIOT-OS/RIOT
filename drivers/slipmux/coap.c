/*
 * SPDX-FileCopyrightText: 2026 HAW Hamburg
 * SPDX-License-Identifier: LGPL-2.1-only
 */

/**
 * @{
 *
 * @file
 * @author  Bennet Hattesen <bennet.hattesen@haw-hamburg.de>
 */
#include "checksum/crc16_ccitt.h"
#include "event.h"
#include "net/unicoap/transport.h"
#include "slipdev.h"
#include "slipdev_internal.h"

/* The special init is the result of normal fcs init combined with slipmux config start (0xa9) */
#define SPECIAL_INIT_FCS (0x374cU)

static event_queue_t *queue = NULL;

static inline void _slipdev_lock(void)
{
    if (IS_USED(MODULE_STDIO_SLIPDEV) || IS_USED(MODULE_SLIPDEV_NET)) {
        mutex_lock(&slipdev_mutex);
    }
}

static inline void _slipdev_unlock(void)
{
    if (IS_USED(MODULE_STDIO_SLIPDEV) || IS_USED(MODULE_SLIPDEV_NET)) {
        mutex_unlock(&slipdev_mutex);
    }
}

/* called in ISR context */
void slipdev_coap_dispatch_recv(event_t *event)
{
    if (queue) {
        event_post(queue, event);
    }
}

void slipdev_coap_set_event_queue(event_queue_t *q)
{
    queue = q;
}

void slipdev_coap_unset_event_queue(void)
{
    queue = NULL;
}

int slipdev_coap_recv(uint8_t *buf, size_t buf_size, slipdev_t *dev)
{
    size_t len;

    if (crb_get_chunk_size(&dev->rb_config, &len)) {
        if ((len > buf_size) || (len <= 2)) {
            crb_consume_chunk(&dev->rb_config, NULL, len);
            return -1;
        }
        crb_consume_chunk(&dev->rb_config, buf, len);

        /* Is the crc correct via residue(=0xF0B8) test */
        if (crc16_ccitt_fcs_update(SPECIAL_INIT_FCS, buf, len) != 0xF0B8) {
            return -2;
        }
        /* cut off the FCS checksum at the end */
        size_t pktlen = len - 2;

        return pktlen;
    }
    return 0;
}

void slipdev_coap_send(uint8_t *buf, size_t buf_size, const slipdev_t *dev)
{
    uint16_t fcs_sum = crc16_ccitt_fcs_finish(SPECIAL_INIT_FCS, buf, buf_size);

    _slipdev_lock();
    slipdev_write_byte(dev->config.uart, SLIPDEV_START_COAP);
    slipdev_write_bytes(dev->config.uart, buf, buf_size);
    slipdev_write_bytes(dev->config.uart, (uint8_t *)&fcs_sum, 2);
    slipdev_write_byte(dev->config.uart, SLIPDEV_END);
    _slipdev_unlock();
}

void slipdev_setup_coap(slipdev_t *dev)
{
    crb_init(&dev->rb_config, dev->rxmem_config, sizeof(dev->rxmem_config));
    dev->rxevent.handler = unicoap_slipdev_recv_handler;
}

/** @} */
